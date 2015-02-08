/*
 * File StatusHandler.h, StatusHandler
 * Created on 08/01/2015.
 * Author: Agustin
 *
 */

#ifndef STATUSHANDLER_H
#define STATUSHANDLER_H


#include <string>
#include <vector>
#include <stack>
#include <cstdint>


#include "Debug.h"



///
/// \brief The StatusHandler class will be used to track all the Status instances
///        but only if they have information associated to them.
///
template<typename Type>
class StatusHandler
{
public:
    ///
    /// \brief Constructr that takes a initial value of elements we will be
    ///        able to handle (error at the same time, it shouldn't be very
    ///        big.
    ///
    inline StatusHandler(unsigned int initSize = 32);

    ///
    /// \brief Used to get a new available entry we can use. This index should
    ///        be returned when is not used anymore (remember to call decrement
    ///        ref)
    /// \return the new available index
    /// \note this method will automatically increment the reference count by 1
    ///
    inline unsigned int
    getAvailableIndex(void);

    ///
    /// \brief Methods used to increment / decrement the references we have
    ///        to an specific entry in the table.
    /// \param index    the index we want to increment / decrement.
    /// \note If we decrement a reference and is the last one then we automatically
    ///       put the index as available and clear the data as well
    ///
    inline void
    incrementRef(unsigned int index);
    inline void
    decrementRef(unsigned int index);

    ///
    /// \brief Returns the associated information we have at a certain position
    /// \param index    The index position
    /// \return reference of the object
    ///
    inline const Type&
    getData(unsigned int index) const;
    inline Type&
    getData(unsigned int index);


private:
    ///
    /// \brief This method should be called when we don't use an index anymore
    /// \param index    the index to return
    /// \note assumes that there is no one else referencing it
    ///
    inline void
    returnAvailableIndex(unsigned int index);

private:
    struct Element {
        Type data;
        std::uint8_t refCounter;

        Element() : refCounter(0) {}
    };


    std::vector<Element> m_elements;
    std::stack<unsigned int> m_indices;

#ifdef DEBUG
    unsigned int m_maxValReach;
#endif

};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template<typename Type>
inline void
StatusHandler<Type>::returnAvailableIndex(unsigned int index)
{
    ASSERT(index < m_elements.size());
    ASSERT(m_elements[index].refCounter == 0);
    m_indices.push(index);

#ifdef DEBUG
    --m_maxValReach;
#endif
}

////////////////////////////////////////////////////////////////////////////////
template<typename Type>
inline StatusHandler<Type>::StatusHandler(unsigned int initSize)
{
    m_elements.resize(initSize);
    for (unsigned int i = 0; i < m_elements.size(); ++i) {
        m_indices.push(i);
    }

#ifdef DEBUG
    m_maxValReach = 0;
#endif
}

template<typename Type>
inline unsigned int
StatusHandler<Type>::getAvailableIndex(void)
{
    ASSERT(!m_indices.empty() && "We can automatically increment here, not for now");
    const unsigned int available = m_indices.top();
    m_indices.pop();

    // increment by one the reference
    incrementRef(available);

#ifdef DEBUG
    ++m_maxValReach;
#endif

    return available;
}

template<typename Type>
inline void
StatusHandler<Type>::incrementRef(unsigned int index)
{
    ASSERT(index < m_elements.size());
    ++(m_elements[index].refCounter);
}

template<typename Type>
inline void
StatusHandler<Type>::decrementRef(unsigned int index)
{
    ASSERT(index < m_elements.size());
    Element& e = m_elements[index];
    --e.refCounter;
    if (e.refCounter == 0) {
        // clear the data as well
        returnAvailableIndex(index);
        e.data.clear();
    }
}

template<typename Type>
inline const Type&
StatusHandler<Type>::getData(unsigned int index) const
{
    ASSERT(index < m_elements.size());
    ASSERT(m_elements[index].refCounter > 0);

    return m_elements[index].data;
}

template<typename Type>
inline Type&
StatusHandler<Type>::getData(unsigned int index)
{
    ASSERT(index < m_elements.size());
    ASSERT(m_elements[index].refCounter > 0);

    return m_elements[index].data;
}


#endif // STATUSHANDLER_H
