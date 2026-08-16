//-----------------------------------------------------------------------------
//  (c) 2012 by Allied Vision Technologies GmbH
//  Project: GenTLValidation
//  Author:  SVW
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef GENTLTESTTIMER_INCLUDE___
#define GENTLTESTTIMER_INCLUDE___

#include "GenTL_v1_4.h"

#include <boost/thread.hpp> 
#include <boost/signals2.hpp> 

class ocGenTLTestTimer
{
    typedef boost::signals2::signal<void (ocGenTLTestTimer*)> timeout_slot;
public:
    typedef timeout_slot::slot_type timeout_slot_t;

public:
    ocGenTLTestTimer() 
        : _interval(0)
        , _is_active(false) 
    {};

    ocGenTLTestTimer(int interval) 
        : _interval(interval)
        , _is_active(false) 
    {};
    
    virtual ~ocGenTLTestTimer() { stop(); };

    inline boost::signals2::connection connect(const timeout_slot_t& subscriber) { return _signalTimeout.connect(subscriber); };

    void start()
    {
        boost::lock_guard<boost::mutex> lock(_guard);

        if (is_active())
            return; // Already executed.
        if (_interval <= 0)
            return;

        _timer_thread.interrupt();
        _timer_thread.join();

        timer_worker job;
        _timer_thread = boost::thread(job, this);

        _is_active = true;
    };

    void stop()
    {
        boost::lock_guard<boost::mutex> lock(_guard);

        if (!is_active())
            return; // Already executed.

        _timer_thread.interrupt();
        _timer_thread.join();

        _is_active = false;
    };

    inline bool is_active() const { return _is_active; };

    inline int get_interval() const { return _interval; };

    void set_interval(const int msec)
    {
        if (msec <= 0 || _interval == msec)
            return;

        boost::lock_guard<boost::mutex> lock(_guard);
        // Keep timer activity status.
        bool was_active = is_active();

        if (was_active)
            stop();
        // Initialize timer with new interval.
        _interval = msec;

        if (was_active)
            start();
    };

    void set_threadid(boost::thread::id id)
    {
        _threadID = id;
    }

    boost::thread::id get_threadid()
    {
        return _threadID;
    }

protected:
    friend struct timer_worker;
    // The timer worker thread.
    struct timer_worker
    {
        void operator()(ocGenTLTestTimer* t)
        {
            boost::posix_time::milliseconds duration(t->get_interval());

            try
            {
                while (1)
                {
                    boost::this_thread::sleep<boost::posix_time::milliseconds>(duration);
                    {
                        boost::this_thread::disable_interruption di;
                        {
                            t->_signalTimeout(t);
                        }
                    }
                }
            }
            catch (boost::thread_interrupted const& )
            {
                // Handle the thread interruption exception.
                // This exception raises on boots::this_thread::interrupt.
            }
        };
    };

protected:
    int             _interval;
    bool            _is_active;

    boost::mutex    _guard;
    boost::thread   _timer_thread;
    boost::thread::id _threadID;

    // Signal slots
    timeout_slot    _signalTimeout;
};

#endif  /* GENTLTESTTIMER_INCLUDE___ */
