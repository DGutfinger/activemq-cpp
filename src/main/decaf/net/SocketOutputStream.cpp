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

#include "SocketOutputStream.h"
#include <decaf/util/Config.h>
#include <decaf/lang/Character.h>
#include "SocketError.h"

using namespace decaf;
using namespace decaf::net;
using namespace decaf::io;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
SocketOutputStream::SocketOutputStream( Socket::SocketHandle socket ) {
    this->socket = socket;
    this->closed = false;
}

////////////////////////////////////////////////////////////////////////////////
SocketOutputStream::~SocketOutputStream() {
    try{
        this->close();
    }
    DECAF_CATCH_NOTHROW( Exception )
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void SocketOutputStream::close() throw( decaf::io::IOException ) {
    this->closed = true;
}

////////////////////////////////////////////////////////////////////////////////
void SocketOutputStream::doWriteByte( unsigned char c ) throw ( IOException ) {

    try{

        // Treat the single byte case the same as an array.
        this->doWriteArrayBounded( &c, 1, 0, 1 );
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCHALL_THROW( IOException )
}

////////////////////////////////////////////////////////////////////////////////
void SocketOutputStream::doWriteArrayBounded( const unsigned char* buffer, std::size_t size,
                                              std::size_t offset, std::size_t length )
    throw ( decaf::io::IOException,
            decaf::lang::exceptions::NullPointerException,
            decaf::lang::exceptions::IndexOutOfBoundsException ) {

    try{

        if( length == 0 ) {
            return;
        }

        if( buffer == NULL ) {
            throw NullPointerException(
                __FILE__, __LINE__,
                "SocketOutputStream::write - passed buffer is null" );
        }

        if( closed ) {
            throw IOException(
                __FILE__, __LINE__,
                "SocketOutputStream::write - This Stream has been closed." );
        }

        if( ( offset + length ) > size ) {
            throw decaf::lang::exceptions::IndexOutOfBoundsException(
                __FILE__, __LINE__,
                "SocketOutputStream::write - given offset + length is greater than buffer size.");
        }

        apr_size_t remaining = (apr_size_t)length;
        apr_status_t result = APR_SUCCESS;

        const unsigned char* lbuffer = buffer + offset;

        while( remaining > 0 && !closed ) {
            // On input remaining is the bytes to send, after return remaining
            // is the amount actually sent.
            result = apr_socket_send( socket, (const char*)lbuffer, &remaining );

            if( result != APR_SUCCESS || closed ) {
                throw IOException(
                    __FILE__, __LINE__,
                    "decaf::net::SocketOutputStream::write - %s",
                    SocketError::getErrorString().c_str() );
            }

            // move us to next position to write, or maybe end.
            lbuffer += remaining;
            remaining = length - remaining;
        }
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCH_RETHROW( NullPointerException )
    DECAF_CATCH_RETHROW( IndexOutOfBoundsException )
    DECAF_CATCHALL_THROW( IOException )
}
