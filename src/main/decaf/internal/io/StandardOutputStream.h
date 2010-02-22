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

#ifndef _DECAF_INTERNAL_IO_STANDARDOUTPUTSTREAM_H_
#define _DECAF_INTERNAL_IO_STANDARDOUTPUTSTREAM_H_

#include <decaf/util/Config.h>
#include <decaf/io/OutputStream.h>

namespace decaf {
namespace internal {
namespace io {

    class DECAF_API StandardOutputStream : public decaf::io::OutputStream {
    public:

        StandardOutputStream();

        virtual ~StandardOutputStream();

        /**
         * Invokes flush on the target output stream.
         * throws IOException if an error occurs
         */
        virtual void flush() throw ( decaf::io::IOException );

        /**
         * Invokes close on the target output stream.
         * throws IOException if an error occurs
         */
        virtual void close() throw( decaf::io::IOException );

    protected:

        virtual void doWriteByte( unsigned char value ) throw ( decaf::io::IOException );

        virtual void doWriteArrayBounded( const unsigned char* buffer, std::size_t size,
                                          std::size_t offset, std::size_t length )
            throw ( decaf::io::IOException,
                    decaf::lang::exceptions::NullPointerException,
                    decaf::lang::exceptions::IndexOutOfBoundsException );

    };

}}}

#endif /* _DECAF_INTERNAL_IO_STANDARDOUTPUTSTREAM_H_ */
