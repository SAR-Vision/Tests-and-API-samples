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

#include <string>

#include "GenApi/GenApi.h"

#include "Impl.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL_MSG(Impl_Test, TestNormalAcquisitionConsumerBuffer, "Impl_Test");
GENTLTEST_UNIT_IMPL(Impl_Test, TestNormalAcquisitionConsumerBufferForAquiredFrames);
GENTLTEST_UNIT_IMPL(Impl_Test, TestNormalAcquisitionConsumerBufferDSGetInfo);
GENTLTEST_UNIT_IMPL(Impl_Test, TestNormalAcquisitionProducerBuffer);
GENTLTEST_UNIT_IMPL(Impl_Test, TestCommandNewDataProducerBuffer);
GENTLTEST_UNIT_IMPL(Impl_Test, TestGCGetPortInfoPortName);
GENTLTEST_UNIT_IMPL(Impl_Test, TestBufferPoolLockDuringAcquisition);

GENTLTEST_UNIT_IMPL_MSG(Impl_Test2, TestAcquisitionConsumerBufferDSGetBufferChunkData, "Impl_Test2");
GENTLTEST_UNIT_IMPL(Impl_Test2, TestAcquisitionProducerBufferDSGetBufferChunkData);
