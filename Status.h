/*
 * File Status.h, Status
 * Created on 08/01/2015.
 * Author: Agustin
 *
 */

#ifndef STATUS_H
#define STATUS_H

#include <cstdint>

#include "Debug.h"
#include "StatusHandler.h"




/// Helper macros to check a specific status if critical and return
///
#define ST_CHECK_CRITICAL(st)\
    if (st.severity() == StatusSeverity::STS_CRITICAL) {\
        return st;\
    }



/// \brief Here we will define the list of common Status we can return (error code)
///
enum StatusCode {
    STC_NO_ERROR        = 0,    // No error
    STC_INTERNAL_ERROR,         // when something really bad happens inside of a method
    STC_INVALID_ARGS,           // something is wrong with the arguments
    STC_NOT_POSSIBLE,           // when we are not able to execute a certain operation
    // ...
};

/// \brief We can assign a severity as well to the status.
///
enum StatusSeverity {
    STS_UNDEFINED   = 0,
    STS_CRITICAL    = 1,
    STS_NORMAL      = 2,
    STS_LOW         = 3,
};

///
/// \brief The Status class
///        The purpose of this class is to provide a better error (with
///        information if needed) handling.
///
class Status
{
public:
    // this variable should be set before anything;
    static StatusHandler<std::string>* s_shandler;
public:

    ///
    /// \brief Different constructor possibilities, depending what we need
    ///        By default we will set to STC_INTERNAL_ERROR since is a good
    ///        practice initialize everything as an error at the begining.
    ///
    explicit inline Status(StatusCode code = StatusCode::STC_INTERNAL_ERROR,
                           StatusSeverity severity = StatusSeverity::STS_UNDEFINED);
    explicit inline Status(StatusCode code,
                           StatusSeverity severity,
                           const char* description);
    explicit inline Status(StatusCode code,
                           StatusSeverity severity,
                           const std::string& description);
    ///
    /// \brief Special constructor from a boolean value,
    ///        when allesGut is true:
    ///         StatusCode = NO ERROR and Severity = Undefined.
    ///        when allesGut == false:
    ///         StatusCode = STC_INTERNAL_ERROR and Severity = STS_NORMAL
    /// \param allesGut     Indicates if everything is ok or not.
    /// \note that here we will not set this constructor as explicit since
    ///       the idea is that if you return true inside of the method then
    ///       will automatically be casted to "no error"
    ///
    inline Status(bool allesGut);

    inline ~Status();

    ///
    /// \brief We need to reimplement this methods since we have a ref counter
    ///        on the handler
    ///
    inline Status(const Status& other);
    inline Status& operator=(const Status& other);

    ///
    /// \brief operator bool
    ///        This is used to automatically detect if the current error code is
    ///        equal to STC_NO_ERROR, in such case true is returned, otherwise
    ///        false is returned.
    ///
    inline operator bool() const;

    ///
    /// \brief Return the current associated status code
    /// \return the current associated status code
    ///
    inline StatusCode
    code(void) const;
    inline void
    changeCode(StatusCode newCode);

    ///
    /// \brief Returns the associated severity of the status
    /// \return the current severity
    ///
    inline StatusSeverity
    severity(void) const;
    inline void
    changeSeverity(StatusSeverity newSeverity);

    ///
    /// \brief This method will return true if we have additional information
    ///        like a string associated to it, false otherwise
    /// \return true if we have additional information | false otherwise
    ///
    inline bool
    containsDescription(void) const;

    ///
    /// \brief Different helper methods to retrieve the additional information
    ///        if we have
    /// \return additional information associated.
    ///
    inline const char*
    description(void) const;
    inline const std::string&
    descriptionStd(void) const;

    ///
    /// \brief Different useful methods to append information to this status
    /// \param desc The information we want to append.
    ///
    inline void
    appendDescription(const std::string& desc);
    inline void
    appendDescription(const char* desc);

    ///
    /// \brief This method will remove the associated description if we have.
    ///
    inline void
    clearDescription(void);

    ///
    /// \brief operator += will be used to mix different status, we will
    ///        concatenate the strings and also maintain the highest priority
    ///        and the error. In case that the error is different, we will
    ///        simply set it to internal error.
    /// \param other    The other status we want to mix with
    /// \return the mixed new status (this)
    ///
    inline Status&
    operator+=(const Status& other);


private:
    // this is all the information we will hold to maintain the same efficiency
    // than normal integers.
    // the scheme as next
    // bits: 0..3   (4 bits)    => Severity (none, critical, normal, low)
    // bits: 4..15  (12 bits)   => StatusCode (error code)
    // bits: 16..31 (16 bits)   => Internal index referencing table

    struct Data {
        // define the invalid index
        static const unsigned short INV_INDEX = (1 << 15);

        unsigned int code       : 12;
        unsigned int severity   : 4;
        unsigned int index      : 16;

        Data(unsigned int c, unsigned int sev, unsigned short i) :
            code(c), severity(sev), index(i)
        {}
    } data;
};














////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline
Status::Status(StatusCode code, StatusSeverity severity) :
    data(code, severity, Data::INV_INDEX)
{
    ASSERT(s_shandler != 0);
}

inline
Status::Status(StatusCode code, StatusSeverity severity, const char* description) :
    data(code, severity, Data::INV_INDEX)
{
    ASSERT(s_shandler != 0);
    // if description is not 0 then we need to assign a new id.
    if (description != 0) {
        appendDescription(description);
    }
}

inline
Status::Status(StatusCode code, StatusSeverity severity, const std::string& description) :
    data(code, severity, Data::INV_INDEX)
{
    ASSERT(s_shandler != 0);
    // if description is not empty then we need to assign a new id.
    if (!description.empty()) {
        appendDescription(description);
    }
}

inline Status::Status(bool allesGut) :
    data(allesGut ? StatusCode::STC_NO_ERROR : StatusCode::STC_INTERNAL_ERROR,
         allesGut ? StatusSeverity::STS_UNDEFINED : StatusSeverity::STS_NORMAL,
         Status::Data::INV_INDEX)
{
}

inline Status::~Status()
{
    // if we have description we need to unreference ourself
    if (containsDescription()) {
        ASSERT(s_shandler != 0);
        s_shandler->decrementRef(data.index);
    }
}

inline Status::Status(const Status& other) :
    data(other.data.code, other.data.severity, other.data.index)
{
    ASSERT(s_shandler != 0);
    if (containsDescription()) {
        // increment reference
        ASSERT(s_shandler != 0);
        s_shandler->incrementRef(data.index);
    }
}

inline Status& Status::operator=(const Status& other)
{
    ASSERT(s_shandler != 0);
    // if we have description we need to remove it
    data = other.data;
    if (containsDescription()) {
        // increment reference
        ASSERT(s_shandler != 0);
        s_shandler->incrementRef(data.index);
    }
    return *this;
}

inline Status::operator bool() const
{
    return data.code == StatusCode::STC_NO_ERROR;
}

inline StatusCode
Status::code(void) const
{
    return static_cast<StatusCode>(data.code);
}
inline void
Status::changeCode(StatusCode newCode)
{
    data.code = newCode;
}

inline StatusSeverity
Status::severity(void) const
{
    return static_cast<StatusSeverity>(data.severity);
}
inline void
Status::changeSeverity(StatusSeverity newSeverity)
{
    data.severity = newSeverity;
}

inline bool
Status::containsDescription(void) const
{
    return data.index != Data::INV_INDEX;
}

inline const char*
Status::description(void) const
{
    // we can enforce here that if we call this method is because we really have
    // information
    if (!containsDescription()) {
        return "";
    }
    ASSERT(s_shandler != 0);
    return s_shandler->getData(data.index).c_str();
}
inline const std::string&
Status::descriptionStd(void) const
{
    ASSERT(containsDescription());
    ASSERT(s_shandler != 0);
    return s_shandler->getData(data.index);
}

inline void
Status::appendDescription(const std::string& desc)
{
    // here we need to check if we have information already or not, if we don't
    // we need to ask for a available place in the handler
    ASSERT(s_shandler != 0);
    if (!containsDescription()) {
        data.index = s_shandler->getAvailableIndex();
    }
    s_shandler->getData(data.index) += desc;
}

inline void
Status::appendDescription(const char* desc)
{
    // here we need to check if we have information already or not, if we don't
    // we need to ask for a available place in the handler
    ASSERT(s_shandler != 0);
    if (!containsDescription()) {
        data.index = s_shandler->getAvailableIndex();
    }
    s_shandler->getData(data.index) += desc;
}

inline void
Status::clearDescription(void)
{
    if (containsDescription()) {
        ASSERT(s_shandler != 0);
        s_shandler->decrementRef(data.index);
    }
    data.index = Data::INV_INDEX;
}

inline Status&
Status::operator+=(const Status& other)
{
    if (other.containsDescription()) {
        appendDescription(other.descriptionStd());
    }
    if (other.data.severity != data.severity) {
        const unsigned int minSevNotZero = data.severity == StatusSeverity::STS_UNDEFINED ?
                                     other.data.severity :
                                     other.data.severity == StatusSeverity::STS_UNDEFINED ?
                                     data.severity :
                                     std::min(other.data.severity, data.severity);
        data.severity = minSevNotZero;
    }
    if (other.data.code != data.code) {
        data.code = StatusCode::STC_INTERNAL_ERROR;
    }
    return *this;
}



#endif // STATUS_H
