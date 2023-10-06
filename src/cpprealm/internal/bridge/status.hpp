#ifndef CPP_REALM_BRIDGE_REALM_STATUS_HPP
#define CPP_REALM_BRIDGE_REALM_STATUS_HPP

#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Status;
    struct ErrorCategory;
}
namespace realm::internal::bridge {

    struct error_category {
        enum type {
            logic_error,
            runtime_error,
            invalid_argument,
            file_access,
            system_error,
            app_error,
            client_error,
            json_error,
            service_error,
            http_error,
            custom_error,
            websocket_error,
            sync_error,
        };
        error_category() = default;
        error_category(error_category&&) = default;
        error_category(const error_category&) = default;
        error_category& operator=(const error_category&) = default;
        error_category& operator=(error_category&&) = default;
        ~error_category() = default;
        bool test(type cat);
        error_category& set(type cat);
        void reset(type cat);
        bool operator==(const error_category& other) const;
        bool operator!=(const error_category& other) const;
        int value() const;

    private:
        unsigned m_error_category = 0;
    };

    class error_codes {
    public:
        // Explicitly 32-bits wide so that non-symbolic values,
        // like uassert codes, are valid.
        enum error : std::int32_t {
            OK,
            RuntimeError,
            RangeError,
            BrokenInvariant,
            OutOfMemory,
            OutOfDiskSpace,
            AddressSpaceExhausted,
            MaximumFileSizeExceeded,
            IncompatibleSession,
            IncompatibleLockFile,
            UnsupportedFileFormatVersion,
            MultipleSyncAgents,
            ObjectAlreadyExists,
            NotCloneable,
            BadChangeset,
            SubscriptionFailed,
            FileOperationFailed,
            PermissionDenied,
            FileNotFound,
            FileAlreadyExists,
            InvalidDatabase,
            DecryptionFailed,
            IncompatibleHistories,
            FileFormatUpgradeRequired,
            SchemaVersionMismatch,
            NoSubscriptionForWrite,
            BadVersion,
            OperationAborted,

            AutoClientResetFailed,
            BadSyncPartitionValue,
            ConnectionClosed,
            InvalidSubscriptionQuery,
            SyncClientResetRequired,
            SyncCompensatingWrite,
            SyncConnectFailed,
            SyncConnectTimeout,
            SyncInvalidSchemaChange,
            SyncPermissionDenied,
            SyncProtocolInvariantFailed,
            SyncProtocolNegotiationFailed,
            SyncServerPermissionsChanged,
            SyncUserMismatch,
            TlsHandshakeFailed,
            WrongSyncType,
            SyncWriteNotAllowed,

            SystemError,

            LogicError,
            NotSupported,
            BrokenPromise,
            CrossTableLinkTarget,
            KeyAlreadyUsed,
            WrongTransactionState,
            WrongThread,
            IllegalOperation,
            SerializationError,
            StaleAccessor,
            InvalidatedObject,
            ReadOnlyDB,
            DeleteOnOpenRealm,
            MismatchedConfig,
            ClosedRealm,
            InvalidTableRef,
            SchemaValidationFailed,
            SchemaMismatch,
            InvalidSchemaVersion,
            InvalidSchemaChange,
            MigrationFailed,
            InvalidQuery,

            BadServerUrl,
            InvalidArgument,
            TypeMismatch,
            PropertyNotNullable,
            ReadOnlyProperty,
            MissingPropertyValue,
            MissingPrimaryKey,
            UnexpectedPrimaryKey,
            ModifyPrimaryKey,
            SyntaxError,
            InvalidProperty,
            InvalidName,
            InvalidDictionaryKey,
            InvalidDictionaryValue,
            InvalidSortDescriptor,
            InvalidEncryptionKey,
            InvalidQueryArg,
            KeyNotFound,
            OutOfBounds,
            LimitExceeded,
            ObjectTypeMismatch,
            NoSuchTable,
            TableNameInUse,
            IllegalCombination,
            TopLevelObject,

            CustomError,

            ClientUserNotFound,
            ClientUserNotLoggedIn,
            ClientAppDeallocated,
            ClientRedirectError,
            ClientTooManyRedirects,

            BadToken,
            MalformedJson,
            MissingJsonKey,
            BadBsonParse,

            MissingAuthReq,
            InvalidSession,
            UserAppDomainMismatch,
            DomainNotAllowed,
            ReadSizeLimitExceeded,
            InvalidParameter,
            MissingParameter,
            TwilioError,
            GCMError,
            HTTPError,
            AWSError,
            MongoDBError,
            ArgumentsNotAllowed,
            FunctionExecutionError,
            NoMatchingRuleFound,
            InternalServerError,
            AuthProviderNotFound,
            AuthProviderAlreadyExists,
            ServiceNotFound,
            ServiceTypeNotFound,
            ServiceAlreadyExists,
            ServiceCommandNotFound,
            ValueNotFound,
            ValueAlreadyExists,
            ValueDuplicateName,
            FunctionNotFound,
            FunctionAlreadyExists,
            FunctionDuplicateName,
            FunctionSyntaxError,
            FunctionInvalid,
            IncomingWebhookNotFound,
            IncomingWebhookAlreadyExists,
            IncomingWebhookDuplicateName,
            RuleNotFound,
            APIKeyNotFound,
            RuleAlreadyExists,
            RuleDuplicateName,
            AuthProviderDuplicateName,
            RestrictedHost,
            APIKeyAlreadyExists,
            IncomingWebhookAuthFailed,
            ExecutionTimeLimitExceeded,
            NotCallable,
            UserAlreadyConfirmed,
            UserNotFound,
            UserDisabled,
            AuthError,
            BadRequest,
            AccountNameInUse,
            InvalidPassword,
            SchemaValidationFailedWrite,
            AppUnknownError,
            MaintenanceInProgress,
            UserpassTokenInvalid,
            InvalidServerResponse,
            AppServerError,

            CallbackFailed,
            UnknownError,
        };

        static error_category error_categories(error code);
        static std::string_view error_string(error code);
        static error from_string(std::string_view str);
        static std::vector<error> get_all_codes();
        static std::vector<std::string_view> get_all_names();
        static std::vector<std::pair<std::string_view, error_codes::error>> get_error_list();
    };

    struct status {

        status(const ::realm::Status&);
        status(::realm::Status&&);
        status(const status&);
        status(status&&);
        status& operator=(const status&);
        status& operator=(status&&);
        ~status();

        inline bool is_ok() const noexcept;
        inline const std::string& reason() const noexcept;
        inline error_codes::error code() const noexcept;
        inline std::string_view code_string() const noexcept;

    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Status m_status[1];
#else
        std::shared_ptr<Status> m_status;
#endif
    };

} // namespace realm::internal::bridge
#endif//CPP_REALM_BRIDGE_REALM_STATUS_HPP
