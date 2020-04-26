/**
 *  @file thread_controller.h
 * 
 *  @date Jan 5, 2019
 *  @author Matthew Kennedy, (c) 2019
 */

#pragma once

#include "global.h"
#include "controller_base.h"

#if ! EFI_UNIT_TEST

/**
 * @brief A base class for a controller that requires its own thread.
 * 
 * Inherit from ThreadController.  Implement ThreadTask with the logic required for your thread.
 * The template parameter specifies the size of the stack used for the thread.  (because we have to
 * allocate the stack at compile time, it has to be a template parameter instead of a normal parameter)
 */
template <int TStackSize>
class ThreadController : public ControllerBase
{
private:
    THD_WORKING_AREA(m_threadstack, TStackSize);
    const tprio_t m_prio;

    /**
     * The OS can only call a function with a single void* param.  We have
     * to convert back to an instance of ThreadController, and call the task to run.
     */
    static void StaticThreadTaskAdapter(void* thread)
    {
        ThreadController* t = static_cast<ThreadController*>(thread);

        // Run our thread task
        t->ThreadTask();
    }

protected:
    /**
     * Override this function to implement your controller's thread's behavior.
     */
    virtual void ThreadTask() = 0;
    thread_t* m_thread;

public:
    ThreadController(const char* name, tprio_t priority)
        : ControllerBase(name)
        , m_prio(priority)
    {
    }

    /**
     * @brief Start the thread.
     */
    void Start()
    {
        m_thread = chThdCreateStatic(m_threadstack, sizeof(m_threadstack), m_prio, StaticThreadTaskAdapter, this);
        m_thread->name = GetName();
    }
};

#endif
