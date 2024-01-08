#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.dll>

using namespace System::Security::Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction::RequestMinimum, SkipVerification=false)];
namespace SharpSvn {
    using namespace System;
    using namespace System;
    ref class SharpSvnStrings;
    
    
    /// <summary>
    ///   Eine stark typisierte Ressourcenklasse zum Suchen von lokalisierten Zeichenfolgen usw.
    /// </summary>
    // Diese Klasse wurde von der StronglyTypedResourceBuilder automatisch generiert
    // -Klasse über ein Tool wie ResGen oder Visual Studio automatisch generiert.
    // Um einen Member hinzuzufügen oder zu entfernen, bearbeiten Sie die .ResX-Datei und führen dann ResGen
    // mit der /str-Option erneut aus, oder Sie erstellen Ihr VS-Projekt neu.
    [System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.Resources.Tools.StronglyTypedResourceBuilder", L"4.0.0.0"), 
    System::Diagnostics::DebuggerNonUserCodeAttribute, 
    System::Runtime::CompilerServices::CompilerGeneratedAttribute]
    ref class SharpSvnStrings {
        
        private: static ::System::Resources::ResourceManager^  resourceMan;
        
        private: static ::System::Globalization::CultureInfo^  resourceCulture;
        
        protected private: [System::Diagnostics::CodeAnalysis::SuppressMessageAttribute(L"Microsoft.Performance", L"CA1811:AvoidUncalledPrivateCode")]
        SharpSvnStrings();
        /// <summary>
        ///   Gibt die zwischengespeicherte ResourceManager-Instanz zurück, die von dieser Klasse verwendet wird.
        /// </summary>
        internal: [System::ComponentModel::EditorBrowsableAttribute(::System::ComponentModel::EditorBrowsableState::Advanced)]
        static property ::System::Resources::ResourceManager^  ResourceManager {
            ::System::Resources::ResourceManager^  get();
        }
        
        /// <summary>
        ///   Überschreibt die CurrentUICulture-Eigenschaft des aktuellen Threads für alle
        ///   Ressourcenzuordnungen, die diese stark typisierte Ressourcenklasse verwenden.
        /// </summary>
        internal: [System::ComponentModel::EditorBrowsableAttribute(::System::ComponentModel::EditorBrowsableState::Advanced)]
        static property ::System::Globalization::CultureInfo^  Culture {
            ::System::Globalization::CultureInfo^  get();
            System::Void set(::System::Globalization::CultureInfo^  value);
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die All targets must either be Uri or Path ähnelt.
        /// </summary>
        internal: static property System::String^  AllTargetsMustBeUriOrPath {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die All provided Uri&apos;s must be on the same server ähnelt.
        /// </summary>
        internal: static property System::String^  AllUrisMustBeOnTheSameServer {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die This argument is not a valid path. A Uri was specified ähnelt.
        /// </summary>
        internal: static property System::String^  ArgumentMustBeAPathNotAUri {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die This argument is not a valid relative path ähnelt.
        /// </summary>
        internal: static property System::String^  ArgumentMustBeAValidRelativePath {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The argument must be a valid absolute repository Uri ähnelt.
        /// </summary>
        internal: static property System::String^  ArgumentMustBeAValidRepositoryUri {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Can only resolve head, number and date revisions ähnelt.
        /// </summary>
        internal: static property System::String^  CanOnlyResolveHeadDateandExplicitRevisions {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Can&apos;t parse command &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  CantParseCommandX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The collection must at least contain one item ähnelt.
        /// </summary>
        internal: static property System::String^  CollectionMustContainAtLeastOneItem {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die You can only crop a node to a depth between empty and files. ähnelt.
        /// </summary>
        internal: static property System::String^  CropToValidDepth {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die A fatal error occurred within the subversion API - (Use Ctrl+C to copy message) ähnelt.
        /// </summary>
        internal: static property System::String^  FatalExceptionInSubversionApi {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die File &apos;{0}&apos; not found ähnelt.
        /// </summary>
        internal: static property System::String^  FileXNotFound {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The handler is not registered at this time ähnelt.
        /// </summary>
        internal: static property System::String^  HandlerIsNotRegisteredAtThisTime {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Invalid character in client name ähnelt.
        /// </summary>
        internal: static property System::String^  InvalidCharacterInClientName {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Invalid SvnRevisionType value specified ähnelt.
        /// </summary>
        internal: static property System::String^  InvalidSvnRevisionTypeValue {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Invalid Uri string ähnelt.
        /// </summary>
        internal: static property System::String^  InvalidUri {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The IO Thread Blocked -- Do you have a deadlock? ähnelt.
        /// </summary>
        internal: static property System::String^  IOThreadBlocked {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die One or more items in the list are null ähnelt.
        /// </summary>
        internal: static property System::String^  ItemInListIsNull {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The loading of the subversion mime mapping file failed ähnelt.
        /// </summary>
        internal: static property System::String^  LoadingMimeTypesMapFileFailed {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die {This line does contain non-Utf8 characters} ähnelt.
        /// </summary>
        internal: static property System::String^  NonUtf8ConvertableLine {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die At revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyAtRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Checked out external at revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyCheckedOutExternalAtRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Checked out revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyCheckedOutRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Exported external at revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyExportedExternalAtRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Exported revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyExportedRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die External at revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyExternalAtRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Removed external &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyExternalXRemoved {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Failed to revert &apos;{0}&apos; -- try updating instead ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyFailedToRevertX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Fetching external item into &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyFetchExternalItemIntoX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die --- Merging differences between repository URLs into &apos;{0}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyMergingDifferencesBetweenRepositoryUrlsIntoX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die --- Merging r{0} into &apos;{1}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyMergingRXIntoY {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die --- Merging r{0} through r{1} into &apos;{2}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyMergingRXToRYIntoZ {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Path &apos;{0}&apos; is no longer a member of a change list. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyPathXIsNoLongerAMemberOfAChangeList {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Path &apos;{0}&apos; is now a member of change list &apos;{1}&apos;. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyPathXIsNowAMemberOfChangeListY {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Performing status on external item at &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyPerformingStatusOnExternalItemAtX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Resolved conflicted state of &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyResolvedConflictedStateOfX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Restored &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyRestoredX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die --- Reverse-merging r{0} into &apos;{1}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyReverseMergingRXIntoY {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die --- Reverse-merging r{0} through r{1} into &apos;{2}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyReverseMergingRXToRYIntoZ {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Reverted &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyRevertedX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Skipped &apos;{0}&apos; - Access denied ähnelt.
        /// </summary>
        internal: static property System::String^  NotifySkippedAccessDeniedX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Skipped missing target &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifySkippedMissingTargetX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Skipped &apos;{0}&apos; - An obstructing working copy found ähnelt.
        /// </summary>
        internal: static property System::String^  NotifySkippedObstructionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Skipped &apos;{0}&apos; ähnelt.
        /// </summary>
        internal: static property System::String^  NotifySkippedX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Skipped &apos;{0}&apos; - Node remains in conflict ähnelt.
        /// </summary>
        internal: static property System::String^  NotifySkippedXRemainsInConflict {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Status against revision: {0} ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyStatusAgainstRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Transmitting file data ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyTransmittingFileData {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Updated external to revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyUpdatedExternalToRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Updated to revision {0}. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyUpdatedToRevisionX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Updating &apos;{0}&apos;: ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyUpdateStartedX {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die &apos;{0}&apos; locked by user &apos;{1}&apos;. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyXLockedByUserY {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die &apos;{0}&apos; unlocked. ähnelt.
        /// </summary>
        internal: static property System::String^  NotifyXUnlocked {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The number of elements in the paths collection does not match the number in revisions ähnelt.
        /// </summary>
        internal: static property System::String^  PathCountDoesNotMatchRevisions {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Path &apos;{0}&apos; contains invalid characters ähnelt.
        /// </summary>
        internal: static property System::String^  PathXContainsInvalidCharacters {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Paths with a length above MAX_PATH (like &apos;{0}&apos;) must be rooted. ähnelt.
        /// </summary>
        internal: static property System::String^  PathXTooLongAndNotRooted {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Please restart this application before continuing.\n\nStack trace:\n{0} ähnelt.
        /// </summary>
        internal: static property System::String^  PleaseRestartThisApplicationBeforeContinuing {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Property name is not valid ähnelt.
        /// </summary>
        internal: static property System::String^  PropertyNameIsNotValid {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The remote session must be open before it can perform commands ähnelt.
        /// </summary>
        internal: static property System::String^  RemoteSessionMustBeOpenToPerformCommand {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Revision must be &gt;= 0 ähnelt.
        /// </summary>
        internal: static property System::String^  RevisionMustBeGreaterThanOrEqualToZero {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Revision number must be at least 0 ähnelt.
        /// </summary>
        internal: static property System::String^  RevisionNumberMustBeAtLeast0 {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The Working revision type is not supported here ähnelt.
        /// </summary>
        internal: static property System::String^  RevisionTypeCantBeWorking {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Revision type must be head, date or a specific revision number ähnelt.
        /// </summary>
        internal: static property System::String^  RevisionTypeMustBeHeadDateOrSpecific {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The session is already open ähnelt.
        /// </summary>
        internal: static property System::String^  SessionAlreadyOpen {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die This function is not available from managed code.  ähnelt.
        /// </summary>
        internal: static property System::String^  SvnAuthManagedPlaceholder {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Svn operation in progress; an SvnClient instance can handle only one command at a time ähnelt.
        /// </summary>
        internal: static property System::String^  SvnClientOperationInProgress {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die Subversion reported a malfunction &apos;{0}&apos; on line {2} of &apos;{1}&apos;. ähnelt.
        /// </summary>
        internal: static property System::String^  SvnMalfunctionPrefix {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The passed target must contain an explicit revision ähnelt.
        /// </summary>
        internal: static property System::String^  TargetMustContainExplicitRevision {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The specified target is not a valid Path target ähnelt.
        /// </summary>
        internal: static property System::String^  TheTargetIsNotAValidPathTarget {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The specified target is not a value Uri or Path target ähnelt.
        /// </summary>
        internal: static property System::String^  TheTargetIsNotAValidUriOrPathTarget {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The specified target is not a valid Uri target ähnelt.
        /// </summary>
        internal: static property System::String^  TheTargetIsNotAValidUriTarget {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The passed Uri is not absolute ähnelt.
        /// </summary>
        internal: static property System::String^  UriIsNotAbsolute {
            System::String^  get();
        }
        
        /// <summary>
        ///   Sucht eine lokalisierte Zeichenfolge, die The value {0} is not a valid {1} ähnelt.
        /// </summary>
        internal: static property System::String^  VerifyEnumFailed {
            System::String^  get();
        }
    };
}
namespace SharpSvn {
    
    
    inline SharpSvnStrings::SharpSvnStrings() {
    }
    
    inline ::System::Resources::ResourceManager^  SharpSvnStrings::ResourceManager::get() {
        if (System::Object::ReferenceEquals(resourceMan, nullptr)) {
            ::System::Resources::ResourceManager^  temp = (gcnew ::System::Resources::ResourceManager(L"SharpSvn.SharpSvnStrings", 
                SharpSvn::SharpSvnStrings::typeid->Assembly));
            resourceMan = temp;
        }
        return resourceMan;
    }
    
    inline ::System::Globalization::CultureInfo^  SharpSvnStrings::Culture::get() {
        return resourceCulture;
    }
    inline System::Void SharpSvnStrings::Culture::set(::System::Globalization::CultureInfo^  value) {
        resourceCulture = value;
    }
    
    inline System::String^  SharpSvnStrings::AllTargetsMustBeUriOrPath::get() {
        return ResourceManager->GetString(L"AllTargetsMustBeUriOrPath", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::AllUrisMustBeOnTheSameServer::get() {
        return ResourceManager->GetString(L"AllUrisMustBeOnTheSameServer", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::ArgumentMustBeAPathNotAUri::get() {
        return ResourceManager->GetString(L"ArgumentMustBeAPathNotAUri", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::ArgumentMustBeAValidRelativePath::get() {
        return ResourceManager->GetString(L"ArgumentMustBeAValidRelativePath", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::ArgumentMustBeAValidRepositoryUri::get() {
        return ResourceManager->GetString(L"ArgumentMustBeAValidRepositoryUri", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::CanOnlyResolveHeadDateandExplicitRevisions::get() {
        return ResourceManager->GetString(L"CanOnlyResolveHeadDateandExplicitRevisions", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::CantParseCommandX::get() {
        return ResourceManager->GetString(L"CantParseCommandX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::CollectionMustContainAtLeastOneItem::get() {
        return ResourceManager->GetString(L"CollectionMustContainAtLeastOneItem", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::CropToValidDepth::get() {
        return ResourceManager->GetString(L"CropToValidDepth", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::FatalExceptionInSubversionApi::get() {
        return ResourceManager->GetString(L"FatalExceptionInSubversionApi", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::FileXNotFound::get() {
        return ResourceManager->GetString(L"FileXNotFound", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::HandlerIsNotRegisteredAtThisTime::get() {
        return ResourceManager->GetString(L"HandlerIsNotRegisteredAtThisTime", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::InvalidCharacterInClientName::get() {
        return ResourceManager->GetString(L"InvalidCharacterInClientName", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::InvalidSvnRevisionTypeValue::get() {
        return ResourceManager->GetString(L"InvalidSvnRevisionTypeValue", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::InvalidUri::get() {
        return ResourceManager->GetString(L"InvalidUri", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::IOThreadBlocked::get() {
        return ResourceManager->GetString(L"IOThreadBlocked", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::ItemInListIsNull::get() {
        return ResourceManager->GetString(L"ItemInListIsNull", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::LoadingMimeTypesMapFileFailed::get() {
        return ResourceManager->GetString(L"LoadingMimeTypesMapFileFailed", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NonUtf8ConvertableLine::get() {
        return ResourceManager->GetString(L"NonUtf8ConvertableLine", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyAtRevisionX::get() {
        return ResourceManager->GetString(L"NotifyAtRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyCheckedOutExternalAtRevisionX::get() {
        return ResourceManager->GetString(L"NotifyCheckedOutExternalAtRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyCheckedOutRevisionX::get() {
        return ResourceManager->GetString(L"NotifyCheckedOutRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyExportedExternalAtRevisionX::get() {
        return ResourceManager->GetString(L"NotifyExportedExternalAtRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyExportedRevisionX::get() {
        return ResourceManager->GetString(L"NotifyExportedRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyExternalAtRevisionX::get() {
        return ResourceManager->GetString(L"NotifyExternalAtRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyExternalXRemoved::get() {
        return ResourceManager->GetString(L"NotifyExternalXRemoved", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyFailedToRevertX::get() {
        return ResourceManager->GetString(L"NotifyFailedToRevertX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyFetchExternalItemIntoX::get() {
        return ResourceManager->GetString(L"NotifyFetchExternalItemIntoX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyMergingDifferencesBetweenRepositoryUrlsIntoX::get() {
        return ResourceManager->GetString(L"NotifyMergingDifferencesBetweenRepositoryUrlsIntoX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyMergingRXIntoY::get() {
        return ResourceManager->GetString(L"NotifyMergingRXIntoY", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyMergingRXToRYIntoZ::get() {
        return ResourceManager->GetString(L"NotifyMergingRXToRYIntoZ", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyPathXIsNoLongerAMemberOfAChangeList::get() {
        return ResourceManager->GetString(L"NotifyPathXIsNoLongerAMemberOfAChangeList", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyPathXIsNowAMemberOfChangeListY::get() {
        return ResourceManager->GetString(L"NotifyPathXIsNowAMemberOfChangeListY", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyPerformingStatusOnExternalItemAtX::get() {
        return ResourceManager->GetString(L"NotifyPerformingStatusOnExternalItemAtX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyResolvedConflictedStateOfX::get() {
        return ResourceManager->GetString(L"NotifyResolvedConflictedStateOfX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyRestoredX::get() {
        return ResourceManager->GetString(L"NotifyRestoredX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyReverseMergingRXIntoY::get() {
        return ResourceManager->GetString(L"NotifyReverseMergingRXIntoY", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyReverseMergingRXToRYIntoZ::get() {
        return ResourceManager->GetString(L"NotifyReverseMergingRXToRYIntoZ", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyRevertedX::get() {
        return ResourceManager->GetString(L"NotifyRevertedX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifySkippedAccessDeniedX::get() {
        return ResourceManager->GetString(L"NotifySkippedAccessDeniedX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifySkippedMissingTargetX::get() {
        return ResourceManager->GetString(L"NotifySkippedMissingTargetX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifySkippedObstructionX::get() {
        return ResourceManager->GetString(L"NotifySkippedObstructionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifySkippedX::get() {
        return ResourceManager->GetString(L"NotifySkippedX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifySkippedXRemainsInConflict::get() {
        return ResourceManager->GetString(L"NotifySkippedXRemainsInConflict", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyStatusAgainstRevisionX::get() {
        return ResourceManager->GetString(L"NotifyStatusAgainstRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyTransmittingFileData::get() {
        return ResourceManager->GetString(L"NotifyTransmittingFileData", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyUpdatedExternalToRevisionX::get() {
        return ResourceManager->GetString(L"NotifyUpdatedExternalToRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyUpdatedToRevisionX::get() {
        return ResourceManager->GetString(L"NotifyUpdatedToRevisionX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyUpdateStartedX::get() {
        return ResourceManager->GetString(L"NotifyUpdateStartedX", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyXLockedByUserY::get() {
        return ResourceManager->GetString(L"NotifyXLockedByUserY", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::NotifyXUnlocked::get() {
        return ResourceManager->GetString(L"NotifyXUnlocked", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::PathCountDoesNotMatchRevisions::get() {
        return ResourceManager->GetString(L"PathCountDoesNotMatchRevisions", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::PathXContainsInvalidCharacters::get() {
        return ResourceManager->GetString(L"PathXContainsInvalidCharacters", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::PathXTooLongAndNotRooted::get() {
        return ResourceManager->GetString(L"PathXTooLongAndNotRooted", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::PleaseRestartThisApplicationBeforeContinuing::get() {
        return ResourceManager->GetString(L"PleaseRestartThisApplicationBeforeContinuing", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::PropertyNameIsNotValid::get() {
        return ResourceManager->GetString(L"PropertyNameIsNotValid", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::RemoteSessionMustBeOpenToPerformCommand::get() {
        return ResourceManager->GetString(L"RemoteSessionMustBeOpenToPerformCommand", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::RevisionMustBeGreaterThanOrEqualToZero::get() {
        return ResourceManager->GetString(L"RevisionMustBeGreaterThanOrEqualToZero", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::RevisionNumberMustBeAtLeast0::get() {
        return ResourceManager->GetString(L"RevisionNumberMustBeAtLeast0", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::RevisionTypeCantBeWorking::get() {
        return ResourceManager->GetString(L"RevisionTypeCantBeWorking", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific::get() {
        return ResourceManager->GetString(L"RevisionTypeMustBeHeadDateOrSpecific", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::SessionAlreadyOpen::get() {
        return ResourceManager->GetString(L"SessionAlreadyOpen", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::SvnAuthManagedPlaceholder::get() {
        return ResourceManager->GetString(L"SvnAuthManagedPlaceholder", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::SvnClientOperationInProgress::get() {
        return ResourceManager->GetString(L"SvnClientOperationInProgress", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::SvnMalfunctionPrefix::get() {
        return ResourceManager->GetString(L"SvnMalfunctionPrefix", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::TargetMustContainExplicitRevision::get() {
        return ResourceManager->GetString(L"TargetMustContainExplicitRevision", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::TheTargetIsNotAValidPathTarget::get() {
        return ResourceManager->GetString(L"TheTargetIsNotAValidPathTarget", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::TheTargetIsNotAValidUriOrPathTarget::get() {
        return ResourceManager->GetString(L"TheTargetIsNotAValidUriOrPathTarget", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::TheTargetIsNotAValidUriTarget::get() {
        return ResourceManager->GetString(L"TheTargetIsNotAValidUriTarget", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::UriIsNotAbsolute::get() {
        return ResourceManager->GetString(L"UriIsNotAbsolute", resourceCulture);
    }
    
    inline System::String^  SharpSvnStrings::VerifyEnumFailed::get() {
        return ResourceManager->GetString(L"VerifyEnumFailed", resourceCulture);
    }
}
