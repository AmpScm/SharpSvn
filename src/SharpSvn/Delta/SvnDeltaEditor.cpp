#include "StdAfx.h"
#include "SvnAll.h"
#include "SvnDeltaEditor.h"

using namespace SharpSvn;
using namespace SharpSvn::Delta;
using namespace SharpSvn::Implementation;

SvnDeltaEditor::SvnDeltaEditor()
{
}

struct directory_baton;
struct file_baton;

struct root_baton
{
	directory_baton* _firstDir;
	gcroot<SvnDeltaEditor^> _editor;

public:
	root_baton(SvnDeltaEditor^ editor)
	{
		_firstDir = nullptr;
		_editor = editor;
	}

	~root_baton();
};

struct directory_baton
{
	root_baton* _root;
	directory_baton* _parentDir;

	file_baton* _firstFile;
	directory_baton* _firstDir;

	directory_baton* _nextDir;
	gcroot<SvnDeltaNode^> _node;

public:
	directory_baton(root_baton* root, directory_baton* parent_dir, SvnDeltaNode^ dir)
	{
		if (!root)
			throw gcnew ArgumentNullException("root");
		else if (!dir)
			throw gcnew ArgumentNullException("dir");

		_root = root;
		_parentDir = parent_dir;

		_firstFile = nullptr;
		_firstDir = nullptr;
		_node = dir;
		if (parent_dir)
		{
			_nextDir = parent_dir->_firstDir;
			parent_dir->_firstDir = this;
		}
		else
		{
			_nextDir = root->_firstDir;
			root->_firstDir = this;
		}
	}

	~directory_baton();
};

struct file_baton
{
	root_baton* _root;
	directory_baton* _parentDir;

	file_baton* _nextFile;
	gcroot<SvnDeltaNode^> _node;

public:
	file_baton(root_baton* root, directory_baton* parent_dir, SvnDeltaNode^ file)
	{
		if (!root)
			throw gcnew ArgumentNullException("root");
		else if (!file)
			throw gcnew ArgumentNullException("file");

		_root = root;
		_parentDir = parent_dir;
		_node = file;

		_nextFile = parent_dir->_firstFile;
		parent_dir->_firstFile = this;
	}

	~file_baton()
	{
		file_baton** nextRef = &_parentDir->_firstFile;

		while(nextRef && *nextRef && *nextRef != this)
			nextRef = &((*nextRef)->_nextFile);

		if(nextRef && *nextRef == this)
			*nextRef = _nextFile;
		else
			throw gcnew InvalidOperationException();

		_nextFile = nullptr;
	}
};

struct base_baton
{
	root_baton* root;
};

root_baton::~root_baton()
{
	while (_firstDir)
		delete _firstDir;
}

directory_baton::~directory_baton()
{
	while(_firstDir)
		delete _firstDir;
	while(_firstFile)
		delete _firstFile;

	// Remove instance from linked list in parent
	directory_baton** nextRef = _parentDir ? &_parentDir->_firstDir : &_root->_firstDir;

	while(nextRef && *nextRef && *nextRef != this)
		nextRef = &((*nextRef)->_nextDir);

	if(nextRef && *nextRef == this)
		*nextRef = _nextDir;
	else
		throw gcnew InvalidOperationException();

	_nextDir = nullptr;
}


static apr_status_t __cdecl delta_editor_cleanup(void *baton)
{
	base_baton* root = (base_baton*)baton;

	if (root && root->root)
	{
		root_baton* rt = root->root;
		root->root = nullptr;
		delete rt;
	}

	return 0;
}

static svn_error_t* __cdecl 
sharpsvn_wrap_set_target_revision(void *edit_baton, svn_revnum_t target_revision, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	base_baton* bt = (base_baton*)edit_baton;

	SvnDeltaSetTargetEventArgs^ args = gcnew SvnDeltaSetTargetEventArgs(target_revision);
	try
	{
		bt->root->_editor->OnSetTarget(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("set_target_revision", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_open_root(void *edit_baton, svn_revnum_t base_revision, apr_pool_t *dir_pool, void **root_bbt)
{
	UNUSED_ALWAYS(dir_pool);
	base_baton* bt = (base_baton*)edit_baton;
	root_baton* root = bt->root;
	SvnDeltaNode^ dirNode;

	try
	{
		dirNode = root->_editor->CreateDirectoryNode(nullptr, nullptr);
		*root_bbt = new directory_baton(root, nullptr, dirNode);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_root/CreateDirectoryNode", e);
	}

	SvnDeltaOpenEventArgs^ args = gcnew SvnDeltaOpenEventArgs(base_revision);
	try
	{
		bt->root->_editor->OnOpen(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_root", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_delete_entry(const char *path, svn_revnum_t revision, void *parent_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	directory_baton* bt = (directory_baton*)parent_baton;

	SvnDeltaDeleteEntryEventArgs^ args = gcnew SvnDeltaDeleteEntryEventArgs(bt->_node, SvnBase::Utf8_PtrToString(path), revision);
	try
	{
		bt->_root->_editor->OnDeleteEntry(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_root", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_add_directory(const char *path, void *parent_baton, const char *copyfrom_path, 
							svn_revnum_t copyfrom_revision, apr_pool_t *dir_pool, void **child_baton)
{
	UNUSED_ALWAYS(dir_pool);
	directory_baton* parent = (directory_baton*)parent_baton;
	directory_baton* child = nullptr;

	SvnDeltaNode^ dirNode;
	SvnDeltaEditor^ editor = parent->_root->_editor;

	try
	{
		dirNode = editor->CreateDirectoryNode(parent->_node, SvnBase::Utf8_PtrToString(path));
		*child_baton = child = new directory_baton(parent->_root, parent, dirNode);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_directory/CreateDirectoryNode", e);
	}

	SvnDeltaDirectoryAddEventArgs^ args = gcnew SvnDeltaDirectoryAddEventArgs(
		dirNode, 
		SvnBase::Utf8_PtrToString(copyfrom_path), 
		copyfrom_revision);

	try
	{
		editor->OnDirectoryAdd(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_directory", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_open_directory(const char *path, void *parent_baton, svn_revnum_t base_revision,
							 apr_pool_t *dir_pool, void **child_baton)
{
	UNUSED_ALWAYS(dir_pool);
	directory_baton* parent = (directory_baton*)parent_baton;
	directory_baton* child = nullptr;

	SvnDeltaNode^ dirNode;
	SvnDeltaEditor^ editor = parent->_root->_editor;

	try
	{
		dirNode = editor->CreateDirectoryNode(parent->_node, SvnBase::Utf8_PtrToString(path));
		*child_baton = child = new directory_baton(parent->_root, parent, dirNode);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_directory/CreateDirectoryNode", e);
	}

	SvnDeltaDirectoryOpenEventArgs^ args = gcnew SvnDeltaDirectoryOpenEventArgs(
		dirNode, 
		base_revision);

	try
	{
		editor->OnDirectoryOpen(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_directory", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_change_dir_prop(void *dir_baton, const char *name, 
							  const svn_string_t *value, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);

	directory_baton* dir = (directory_baton*)dir_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;

	SvnDeltaDirectoryPropertyChangeEventArgs^ args = gcnew SvnDeltaDirectoryPropertyChangeEventArgs(dir->_node, name, value);

	try
	{
		editor->OnDirectoryPropertyChange(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("change_dir_prop", e);
	}
	finally
	{
		args->Detach(false);
	}

	return nullptr;
}

static svn_error_t* __cdecl 
sharpsvn_wrap_close_directory(void *dir_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	directory_baton* dir = (directory_baton*)dir_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;

	SvnDeltaDirectoryCloseEventArgs^ args = gcnew SvnDeltaDirectoryCloseEventArgs(dir->_node);
	try
	{
		editor->OnDirectoryClose(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_directory", e);
	}
	finally
	{
		args->Detach(false);

		delete dir;
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_absent_directory(const char *path, void *parent_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	directory_baton* dir = (directory_baton*)parent_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;
	
	SvnDeltaDirectoryAbsentEventArgs^ args = nullptr;
	try
	{
		args = gcnew SvnDeltaDirectoryAbsentEventArgs(
			editor->CreateDirectoryNode(dir->_node, SvnBase::Utf8_PtrToString(path)));

		editor->OnDirectoryAbsent(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("absent_directory", e);
	}
	finally
	{
		if (args)
			args->Detach(false);

		delete dir;
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_add_file(const char *path, void *parent_baton, const char *copyfrom_path,
					   svn_revnum_t copyfrom_revision, apr_pool_t *file_pool, void **file_bt)
{
	UNUSED_ALWAYS(file_pool);
	directory_baton* dir = (directory_baton*)parent_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;

	SvnDeltaNode^ fileNode;
	file_baton* child;

	try
	{
		fileNode = editor->CreateFileNode(dir->_node, SvnBase::Utf8_PtrToString(path));
		*file_bt = child = new file_baton(dir->_root, dir, fileNode);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_file/CreateFileNode", e);
	}

	SvnDeltaFileAddEventArgs^ args = gcnew SvnDeltaFileAddEventArgs(
		fileNode, 
		SvnBase::Utf8_PtrToString(copyfrom_path), 
		copyfrom_revision);

	try
	{
		editor->OnFileAdd(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_file", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_open_file(const char *path, void *parent_baton, svn_revnum_t base_revision,
						apr_pool_t *file_pool, void **file_bt)
{
	UNUSED_ALWAYS(file_pool);
	directory_baton* dir = (directory_baton*)parent_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;

	SvnDeltaNode^ fileNode;
	file_baton* child;

	try
	{
		fileNode = editor->CreateFileNode(dir->_node, SvnBase::Utf8_PtrToString(path));
		*file_bt = child = new file_baton(dir->_root, dir, fileNode);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_file/CreateFileNode", e);
	}

	SvnDeltaFileOpenEventArgs^ args = gcnew SvnDeltaFileOpenEventArgs(
		fileNode, 
		base_revision);

	try
	{
		editor->OnFileOpen(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("open_file", e);
	}
	finally
	{
		args->Detach(false);
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_apply_textdelta(void *file_bt, const char *base_checksum, apr_pool_t *pool,
							  svn_txdelta_window_handler_t *handler, void **handler_baton)
{
	UNUSED_ALWAYS(pool);
	file_baton* file = (file_baton*)file_bt;
	SvnDeltaEditor^ editor = file->_root->_editor;

	*handler = svn_delta_noop_window_handler;
	*handler_baton = nullptr;

	SvnDeltaBeforeFileDeltaEventArgs^ args = gcnew SvnDeltaBeforeFileDeltaEventArgs(file->_node, base_checksum);

	try
	{
		editor->OnBeforeFileDelta(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("apply_textdelta", e);
	}
	finally
	{
		args->Detach(false);
	}

	return nullptr;
}

static svn_error_t* __cdecl 
sharpsvn_wrap_change_file_prop(void *file_bt, const char *name, 
							   const svn_string_t *value, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	file_baton* file = (file_baton*)file_bt;
	SvnDeltaEditor^ editor = file->_root->_editor;

	SvnDeltaFilePropertyChangeEventArgs^ args = gcnew SvnDeltaFilePropertyChangeEventArgs(file->_node, name, value);

	try
	{
		editor->OnFilePropertyChange(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("change_file_prop", e);
	}
	finally
	{
		args->Detach(false);
	}

	return nullptr;
}

static svn_error_t* __cdecl 
sharpsvn_wrap_close_file(void *file_bt, const char *text_checksum,
						 apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	file_baton* file = (file_baton*)file_bt;
	SvnDeltaEditor^ editor = file->_root->_editor;

	SvnDeltaFileCloseEventArgs^ args = gcnew SvnDeltaFileCloseEventArgs(file->_node, text_checksum);
	try
	{
		editor->OnFileClose(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("add_directory", e);
	}
	finally
	{
		args->Detach(false);

		delete file;
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_absent_file(const char *path, void *parent_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	directory_baton* dir = (directory_baton*)parent_baton;
	SvnDeltaEditor^ editor = dir->_root->_editor;
	
	SvnDeltaFileAbsentEventArgs^ args = nullptr;
	try
	{
		 args = gcnew SvnDeltaFileAbsentEventArgs(
			 editor->CreateFileNode(dir->_node, SvnBase::Utf8_PtrToString(path)));

		editor->OnFileAbsent(args);

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("absent_directory", e);
	}
	finally
	{
		args->Detach(false);

		delete dir;
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_close_edit(void *edit_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	base_baton* bt = (base_baton*)edit_baton;
	SvnDeltaEditor^ editor = nullptr;

	root_baton* r= bt->root;
	bt->root = nullptr;

	if (r)
	{
		editor = r->_editor;
		delete r;
	}

	try
	{
		if (editor)
			editor->OnClose(gcnew SvnDeltaCloseEventArgs());

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("close_edit", e);
	}
	finally
	{
	}
}

static svn_error_t* __cdecl 
sharpsvn_wrap_abort_edit(void *edit_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	base_baton* bt = (base_baton*)edit_baton;
	SvnDeltaEditor^ editor = nullptr;

	root_baton* r= bt->root;
	bt->root = nullptr;

	if (r)
	{
		editor = r->_editor;
		delete r;
	}

	try
	{
		if (editor)
			editor->OnAbort(gcnew SvnDeltaAbortEventArgs());

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("abort_edit", e);
	}
	finally
	{
	}
}

svn_delta_editor_t* SvnDeltaEditor::AllocEditor(void** baton, AprPool^ pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	svn_delta_editor_t *editor = svn_delta_default_editor(pool->Handle);

	editor->set_target_revision = sharpsvn_wrap_set_target_revision;
	editor->open_root = sharpsvn_wrap_open_root;
	editor->delete_entry = sharpsvn_wrap_delete_entry;
	editor->add_directory = sharpsvn_wrap_add_directory;
	editor->open_directory = sharpsvn_wrap_open_directory;
	editor->change_dir_prop = sharpsvn_wrap_change_dir_prop;
	editor->close_directory = sharpsvn_wrap_close_directory;
	editor->absent_directory = sharpsvn_wrap_absent_directory;
	editor->add_file = sharpsvn_wrap_add_file;
	editor->open_file = sharpsvn_wrap_open_file;
	editor->apply_textdelta = sharpsvn_wrap_apply_textdelta;
	editor->change_file_prop = sharpsvn_wrap_change_file_prop;
	editor->close_file = sharpsvn_wrap_close_file;
	editor->absent_file = sharpsvn_wrap_absent_file;
	editor->close_edit = sharpsvn_wrap_close_edit;
	editor->abort_edit = sharpsvn_wrap_abort_edit;

	base_baton* bt = (base_baton*)pool->AllocCleared(sizeof(base_baton));
	*baton = bt;

	apr_pool_cleanup_register(pool->Handle, bt, delta_editor_cleanup, delta_editor_cleanup);

	bt->root = new root_baton(this);

	return editor;
}

SvnDeltaNode^ SvnDeltaEditor::CreateDirectoryNode(SvnDeltaNode^ parent, String^ name)
{
	return gcnew SvnDeltaNode(name, parent);
}

SvnDeltaNode^ SvnDeltaEditor::CreateFileNode(SvnDeltaNode^ parent, String^ name)
{
	return gcnew SvnDeltaNode(name, parent);
}