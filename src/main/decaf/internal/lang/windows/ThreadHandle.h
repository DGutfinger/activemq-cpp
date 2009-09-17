/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef _DECAF_LANG_THREADHANDLE_H_
#define _DECAF_LANG_THREADHANDLE_H_

#include <decaf/util/Config.h>

namespace decaf {
namespace lang {

    /**
     * Platform definition of a Thread Handle on Windows, contains the type
     * constructs and additional data necessary to implement a thread on
     * a Windows based OS.
     *
     * @since 1.0
     */
    class DECAF_API ThreadHandle {
    public:

        typedef void (*threadEntry)( decaf::lang::ThreadHandle* self, void* data );

        ThreadHandle() {
            running = false;
            returnStatus = false;
            userArg = NULL;
            entryFunctionPtr = NULL;
            handle = NULL;
        }

        ~ThreadHandle() {
            ::CloseHandle( handle );
        }

        HANDLE handle;
        bool returnStatus;
        threadEntry entryFunctionPtr;
        void* userArg;
        bool running;

    };

}}

#endif /* _DECAF_LANG_THREADHANDLE_H_ */
