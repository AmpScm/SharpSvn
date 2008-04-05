// $Id: SvnEnums.h 500 2008-03-26 17:47:22Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientArgs.h"
#include "SvnDiff.h"

namespace SharpSvn {
	namespace Diff {
		using namespace SharpSvn;

		public ref class SvnFileDiffArgs : SvnClientArgs
		{
			SvnIgnoreSpacing _ignoreSpacing;
			bool _ignoreLineEndings;
			bool _showCFunction;

		public:
			property SvnIgnoreSpacing IgnoreSpacing
			{
				SvnIgnoreSpacing get()
				{
					return _ignoreSpacing;
				}
				void set(SvnIgnoreSpacing value)
				{
					_ignoreSpacing = EnumVerifier::Verify(value);
				}
			}

			property bool IgnoreLineEndings
			{
				bool get()
				{
					return _ignoreLineEndings;
				}
				void set(bool value)
				{
					_ignoreLineEndings = value;
				}
			}

			property bool ShowCFunction
			{
				bool get()
				{
					return _showCFunction;
				}
				void set(bool value)
				{
					_showCFunction = value;
				}
			}

		public:
			/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
			virtual property SvnCommandType CommandType 
			{
				SvnCommandType get() override sealed
				{
					return SvnCommandType::Unknown;
				}
			}
		};

		public ref class SvnDiffWriteMergedArgs: SvnClientArgs
		{
			String^ _originalPath;
			String^ _modifiedPath;
			String^ _latestPath;
			String^ _ancestorPath;
			String^ _conflictModified;
			String^ _conflictLatest;
			String^ _conflictSeparator;
			bool _showOriginalInConflict;
			bool _showResolvedConflicts;

		public:
			/// <summary>Gets or sets a replacement for the Original Path (null is use original)</summary>
			property String^ OriginalPath
			{
				String^ get()
				{
					return _originalPath; 
				}
				void set(String^ value)
				{
					_originalPath = value;
				}
			}

			/// <summary>Gets or sets a replacement for the Modified Path (null is use original)</summary>
			property String^ ModifiedPath
			{
				String^ get()
				{
					return _modifiedPath; 
				}
				void set(String^ value)
				{
					_modifiedPath = value;
				}
			}

			/// <summary>Gets or sets a replacement for the Latest Path (null is use original)</summary>
			property String^ LatestPath
			{
				String^ get()
				{
					return _latestPath; 
				}
				void set(String^ value)
				{
					_latestPath = value;
				}
			}

			/// <summary>Gets or sets a replacement for the Ancestor Path (null is use original)</summary>
			property String^ AncestorPath
			{
				String^ get()
				{
					return _ancestorPath; 
				}
				void set(String^ value)
				{
					_ancestorPath = value;
				}
			}

			/// <summary>Gets or sets a replacement for the Conflict modified line (null is use original)</summary>
			property String^ ConflictModified
			{
				String^ get()
				{
					return _conflictModified; 
				}
				void set(String^ value)
				{
					_conflictModified = value;
				}
			}

			/// <summary>Gets or sets a replacement for the Conflict latest line (null is use original)</summary>
			property String^ ConflictLatest
			{
				String^ get()
				{
					return _conflictLatest; 
				}
				void set(String^ value)
				{
					_conflictLatest = value;
				}
			}

			/// <summary>Gets or sets a replacement for the conflict separator (null is use original)</summary>
			property String^ ConflictSeparator
			{
				String^ get()
				{
					return _conflictSeparator; 
				}
				void set(String^ value)
				{
					_conflictSeparator = value;
				}
			}

			property bool ShowOriginalInConflict
			{
				bool get()
				{
					return _showOriginalInConflict;
				}
				void set(bool value)
				{
					_showOriginalInConflict = value;
				}
			}

			property bool ShowResolvedConflicts
			{
				bool get()
				{
					return _showResolvedConflicts;
				}
				void set(bool value)
				{
					_showResolvedConflicts = value;
				}
			}

		public:
			/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
			virtual property SvnCommandType CommandType 
			{
				SvnCommandType get() override sealed
				{
					return SvnCommandType::Unknown;
				}
			}
		};

		public ref class SvnDiffWriteDifferencesArgs : SvnClientArgs
		{
			String^ _originalPath;
			String^ _modifiedPath;
			String^ _originalHeader;
			String^ _modifiedHeader;
			String^ _header;
			String^ _relativeFrom;
			bool _showCFunction;

		public:
			property String^ OriginalHeader
			{
				String^ get()
				{
					return _originalHeader; 
				}
				void set(String^ value)
				{
					_originalHeader = value;
				}
			}

			property String^ ModifiedHeader
			{
				String^ get()
				{
					return _modifiedHeader; 
				}
				void set(String^ value)
				{
					_modifiedHeader = value;
				}
			}			

			property String^ Header
			{
				String^ get()
				{
					return _header; 
				}
				void set(String^ value)
				{
					_header = value;
				}
			}

			property String^ RelativeToPath
			{
				String^ get()
				{
					return _relativeFrom;
				}
				void set(String^ value)
				{
					if (String::IsNullOrEmpty(value))
						_relativeFrom = nullptr;
					else
						_relativeFrom = value;
				}
			}


			property bool ShowCFunction
			{
				bool get()
				{
					return _showCFunction;
				}
				void set(bool value)
				{
					_showCFunction = value;
				}
			}

		public:
			/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
			virtual property SvnCommandType CommandType 
			{
				SvnCommandType get() override sealed
				{
					return SvnCommandType::Unknown;
				}
			}
		};

		public ref class SvnFileDiff sealed : public SvnDiff<String^>
		{
			const char* _originalPath;
			const char* _modifiedPath;
			const char* _latestPath;
			const char* _ancestorPath;
			/// <summary>originalPath and modifiedPath must live at least as long as pool</summary>
			SvnFileDiff(svn_diff_t *diff, const char* originalPath, const char* modifiedPath,
				const char* latestPath, const char* ancestorPath, AprPool^ pool);

		public:
			static bool TryCreate(String^ originalPath, String^ modifiedPath, SvnFileDiffArgs^ args, [Out] SvnFileDiff^% diff);
			static bool TryCreate(String^ originalPath, String^ modifiedPath, String^ latestPath, SvnFileDiffArgs^ args, [Out] SvnFileDiff^% diff);
			static bool TryCreate(String^ originalPath, String^ modifiedPath, String^ latestPath, String^ ancestorPath, SvnFileDiffArgs^ args, [Out] SvnFileDiff^% diff);

		public:
			bool WriteMerged(Stream^ toStream, SvnDiffWriteMergedArgs^ args);
			bool WriteDifferences(Stream^ toStream, SvnDiffWriteDifferencesArgs^ args);
		};
	}
}