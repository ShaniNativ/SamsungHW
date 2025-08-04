/***
Written by: Shani Davidian
Reviewed by: Amit
Date: 18.3.25
Exercise: Dispatcher
***/

#ifndef ILRD_RD1645_DISPATCHER_HPP
#define ILRD_RD1645_DISPATCHER_HPP

#include <list>

namespace ilrd
{

    template <typename EVENT>
    class Dispatcher;

    template <typename EVENT>
    class Acallback;

    template <typename T, typename EVENT>
    class Callback;

    template <typename EVENT>
    class Dispatcher
    {

    public:
        ~Dispatcher();
        Dispatcher() = default;
        Dispatcher(const Dispatcher &other) = delete;
        Dispatcher &operator=(const Dispatcher &other) = delete;
        void Register(Acallback<EVENT>* obj);
        void Unregister(Acallback<EVENT>* obj);
        void Notify(EVENT event); //&

    private:
        std::list<Acallback<EVENT>*> m_list_callbacks;
    };

    template <typename EVENT>
    class Acallback
    {
    public:
        virtual ~Acallback();
        Acallback() = default;
        Acallback(const Acallback* other) = delete;
        Acallback &operator=(const Acallback* other) = delete;
        virtual void Notify(EVENT event) = 0;
        virtual void NotifyDeath() = 0;

    private:
        friend class Dispatcher<EVENT>;
        Dispatcher<EVENT>* m_dispatcher;
        void SetDispatcher(Dispatcher<EVENT>* dispatcher);
    };

    template <typename T, typename EVENT>
    class Callback : public Acallback<EVENT>
    {
    public:
        typedef void (T::*ptr_notify_member_t)(EVENT);
        typedef void (T::*ptr_notify_death_member_t)();
        Callback(T &obj, ptr_notify_member_t notify_member,
                 ptr_notify_death_member_t notify_death_member = nullptr);
        void Notify(EVENT event);
        void NotifyDeath();

    private:
        T &m_obj;
        ptr_notify_member_t m_notify;
        ptr_notify_death_member_t m_death_notify;
    };

    /*========================= IMPLEMENTATION ==================================*/
    /*============================ DISPATCHER ===================================*/

    template <class EVENT>
    ilrd::Dispatcher<EVENT>::~Dispatcher()
    {
        try
        {
            std::list<Acallback<EVENT> *> temp_list(m_list_callbacks);

            for (auto it = temp_list.begin(); it != temp_list.end(); ++it)
            {
                (*it)->NotifyDeath();
                (*it)->SetDispatcher(nullptr);
            }
        }
        catch (...)
        {
        }
    }

    template <class EVENT>
    void ilrd::Dispatcher<EVENT>::Register(ilrd::Acallback<EVENT>* obj)
    {
        m_list_callbacks.push_back(obj);
        obj->SetDispatcher(this);
    }

    template <class EVENT>
    void ilrd::Dispatcher<EVENT>::Unregister(ilrd::Acallback<EVENT>* obj)
    {
        m_list_callbacks.remove(obj);
    }

    template <class EVENT>
    void ilrd::Dispatcher<EVENT>::Notify(EVENT event)
    {
        std::list<Acallback<EVENT> *> temp_list(m_list_callbacks);

        for (auto it = temp_list.begin(); it != temp_list.end(); ++it)
        {
            (*it)->Notify(event);
        }
    }

    /*============================ ACallback ===================================*/

    template <class EVENT>
    ilrd::Acallback<EVENT>::~Acallback()
    {
        if (m_dispatcher)
        {
            m_dispatcher->Unregister(this);
        }
    }

    template <class EVENT>
    void ilrd::Acallback<EVENT>::SetDispatcher(Dispatcher<EVENT> *dispatcher)
    {
        m_dispatcher = dispatcher;
    }

    /*============================ Callback ===================================*/

    template <class T, class EVENT>
    ilrd::Callback<T, EVENT>::Callback(T &obj, ptr_notify_member_t notify_member,
                                       ptr_notify_death_member_t notify_death_member) : m_obj(obj),
                                                                                        m_notify(notify_member), m_death_notify(notify_death_member)
    {
    }

    template <class T, class EVENT>
    void ilrd::Callback<T, EVENT>::Notify(EVENT event)
    {
        (m_obj.*m_notify)(event);
    }

    template <class T, class EVENT>
    void ilrd::Callback<T, EVENT>::NotifyDeath()
    {
        if (nullptr != m_death_notify)
        {
            (m_obj.*m_death_notify)();
        }
    }

} // namespace ilrd
#endif // ILRD_RD1645_DISPATCHER_HPP