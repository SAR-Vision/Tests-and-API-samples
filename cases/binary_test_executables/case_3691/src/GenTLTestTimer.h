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
#include "GenTLTest_Win.h"
#include "AccessLock.h"

class ocGenTLTestTimer
{
    typedef void (timeout_slot)(ocGenTLTestTimer*);

public:
    ocGenTLTestTimer() 
        : _interval(0)
        , _is_active(false) 
        , _stop_signal(false)
        , _stopped_signal(false)
    {
    };

    ocGenTLTestTimer(int interval) 
        : _interval(interval)
        , _is_active(false) 
        , _stop_signal(false)
        , _stopped_signal(false)
    {
    };
    
    virtual ~ocGenTLTestTimer() 
    { 
        stop(); 
    };

    inline void connect(timeout_slot* subscriber) 
    { 
        _signalTimeout = subscriber; 
    };

    void start()
    {
        _stop_signal = false;
        _stopped_signal = false;

        AccessGuard lock(m_oMutex);

        if (is_active())
            return; // Already executed.
        if (_interval <= 0)
            return;

        _timer_thread_handle = CreateThread(NULL, NULL, _ThreadFunction, this, NULL, &_timer_threadID);
        
        _is_active = true;
    };

    void stop()
    {
        _stop_signal = true;

        AccessGuard lock(m_oMutex);

        if (!is_active())
            return; // Already executed.

        while (!_stopped_signal)
            usleep(10);

        _is_active = false;

        CloseHandle(_timer_thread_handle);
    };

    inline bool is_active() const { return _is_active; };

    inline int get_interval() const { return _interval; };

    void set_interval(const int msec)
    {
        if (msec <= 0 || _interval == msec)
            return;

        AccessGuard lock(m_oMutex);
        // Keep timer activity status.
        bool was_active = is_active();

        if (was_active)
            stop();
        // Initialize timer with new interval.
        _interval = msec;

        if (was_active)
            start();
    };

    void set_threadid(uint32_t id)
    {
        _threadID = id;
    }

    uint32_t get_threadid()
    {
        return _threadID;
    }

    bool is_stopsignal()
    {
        return _stop_signal;
    }

    void set_stoppedsignal()
    {
        _stopped_signal = true;
    }

protected:
    
    friend unsigned long __stdcall _ThreadFunction(void *aContext)
    {
        ocGenTLTestTimer* t=(ocGenTLTestTimer*)aContext;
        uint32_t duration=t->get_interval();

        while (!t->is_stopsignal())
        {
            uint32_t count=0;
            while (!t->is_stopsignal() && count++ < (duration/10))
                usleep(10);
            if (!t->is_stopsignal() && count >= (duration/10))
            {
                t->_signalTimeout(t);
            }
        }

        t->set_stoppedsignal();

        return 0;
    };

protected:
    int             _interval;
    bool            _is_active;
    bool            _stop_signal;
    bool            _stopped_signal;

    HANDLE          _timer_thread_handle;
    unsigned long   _timer_threadID;
    uint32_t        _threadID;

    // Signal slots
    timeout_slot    *_signalTimeout;

    AccessLock      m_oMutex;
};

#endif  /* GENTLTESTTIMER_INCLUDE___ */
