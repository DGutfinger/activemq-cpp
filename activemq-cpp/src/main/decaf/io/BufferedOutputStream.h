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

#ifndef _DECAF_IO_BUFFEREDOUTPUTSTREAM_H_
#define _DECAF_IO_BUFFEREDOUTPUTSTREAM_H_

#include <decaf/io/FilterOutputStream.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>

namespace decaf{
namespace io{

    /**
     * Wrapper around another output stream that buffers
     * output before writing to the target output stream.
     */
    class DECAF_API BufferedOutputStream : public FilterOutputStream {
    private:

        /**
         * The internal buffer.
         */
        unsigned char* buffer;

        /**
         * The size of the internal buffer.
         */
        std::size_t bufferSize;

        /**
         * The current head of the buffer.
         */
        std::size_t head;

        /**
         * The current tail of the buffer.
         */
        std::size_t tail;

    public:

        /**
         * Constructor.
         *
         * @param stream
         *      The target output stream.
         * @param own
         *      Indicates if this class owns the stream pointer.
         */
        BufferedOutputStream( OutputStream* stream, bool own = false );

        /**
         * Constructor.
         *
         * @param stream
         *      The target output stream.
         * @param bufferSize
         *      The size for the internal buffer.
         * @param own
         *      Indicates if this class owns the stream pointer.
         */
        BufferedOutputStream( OutputStream* stream, std::size_t bufferSize, bool own = false );

        virtual ~BufferedOutputStream();

        /**
         * Invokes flush on the target output stream.
         * @throws IOException thrown if an error occurs.
         */
        virtual void flush() throw ( decaf::io::IOException );

    protected:

        virtual void doWriteByte( unsigned char c ) throw ( decaf::io::IOException );

        virtual void doWriteArray( const unsigned char* buffer, std::size_t size )
            throw ( decaf::io::IOException );

        virtual void doWriteArrayBounded( const unsigned char* buffer, std::size_t size,
                                          std::size_t offset, std::size_t length )
            throw ( decaf::io::IOException,
                    decaf::lang::exceptions::NullPointerException,
                    decaf::lang::exceptions::IndexOutOfBoundsException );

    private:

        /**
         * Initializes the internal structures.
         * @param bufferSize
         *      How large to make the initial buffer when creating it.
         */
        void init( std::size_t bufferSize );

        /**
         * Writes the contents of the buffer to the output stream.
         */
        void emptyBuffer() throw ( decaf::io::IOException );

   };

}}

#endif /*_DECAF_IO_BUFFEREDOUTPUTSTREAM_H_*/
