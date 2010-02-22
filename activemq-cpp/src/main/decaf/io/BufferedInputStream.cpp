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

#include "BufferedInputStream.h"

#include <decaf/lang/System.h>

#include <algorithm>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace decaf{
namespace io{

    class StreamBuffer {
    private:

        unsigned char* buffer;
        std::size_t bufferSize;
        std::size_t pos;
        std::size_t count;
        std::size_t markLimit;
        long long markPos;

    public:

        StreamBuffer( std::size_t bufferSize ) {

            this->buffer = new unsigned char[bufferSize];
            this->bufferSize = bufferSize;
            this->pos = 0;
            this->count = 0;
            this->markLimit = 0;
            this->markPos = -1;
        }

        ~StreamBuffer() {
            delete [] this->buffer;
        }

        void resize( std::size_t newSize ) {
            unsigned char* temp = new unsigned char[newSize];
            System::arraycopy( temp, 0, buffer, 0, count );
            std::swap( temp, buffer );
            delete [] temp;
            this->bufferSize = newSize;
        }

        std::size_t getUnusedBytes() const{
            return bufferSize - count;
        }

        std::size_t available() const {
            return this->count - this->pos;
        }

        unsigned char next() {
            return this->buffer[this->pos++];
        }

        void advance( std::size_t amount ) {
            this->pos += amount;
        }

        void reverse( std::size_t amount ) {
            this->pos -= amount;
        }

        void advanceTail( std::size_t amount ) {
            this->count += amount;
        }

        std::size_t getBufferSize() {
            return this->bufferSize;
        }

        unsigned char* getBuffer() {
            return this->buffer;
        }

        std::size_t getCount() const{
            return count;
        }

        void setCount( std::size_t count ) {
            this->count = count;
        }

        std::size_t getPos() const{
            return pos;
        }

        void setPos( std::size_t pos ) {
            this->pos = pos;
        }

        void clear(){
            pos = count = 0;
        }

        bool isEmpty() const{
            return pos == count;
        }

        bool isMarked() const{
            return this->markPos != -1;
        }

        void reset() {
            this->pos = this->markPos;
        }

        void normalizeBuffer() {
            if( isEmpty() ){
                clear();
            }
        }

        void mark( int readLimit ) {
            this->markLimit = readLimit;
            this->markPos = this->pos;
        }

        void unmark() {
            this->markLimit = 0;
            this->markPos = -1;
        }

        bool isReadLimitExceeded() const {
            return pos - markPos >= markLimit;
        }

        std::size_t getMarkPos() const {
            return (std::size_t)this->markPos;
        }

        void setMarkPos( std::size_t markPos ) {
            this->markPos = markPos;
        }

        std::size_t getMarkLimit() const {
            return this->markLimit;
        }
    };

}}

////////////////////////////////////////////////////////////////////////////////
BufferedInputStream::BufferedInputStream( InputStream* stream, bool own )
: FilterInputStream( stream, own ) {

    // Default to a 8k buffer.
    this->buffer = new StreamBuffer( 8192 );
}

////////////////////////////////////////////////////////////////////////////////
BufferedInputStream::BufferedInputStream( InputStream* stream, std::size_t bufferSize, bool own )
    throw ( lang::exceptions::IllegalArgumentException ) : FilterInputStream( stream, own ) {

    if( bufferSize == 0 ) {
        throw new IllegalArgumentException(
            __FILE__, __LINE__,
            "BufferedInputStream::init - Size must be greater than zero");
    }

    this->buffer = new StreamBuffer( bufferSize );
}

////////////////////////////////////////////////////////////////////////////////
BufferedInputStream::~BufferedInputStream() {
    try{
        this->close();
        delete this->buffer;
    }
    DECAF_CATCH_NOTHROW( IOException )
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void BufferedInputStream::close() throw( IOException ) {

    // let parent close the inputStream
    FilterInputStream::close();
}

////////////////////////////////////////////////////////////////////////////////
std::size_t BufferedInputStream::available() const throw ( IOException ) {

    if( buffer == NULL || this->isClosed() ) {
        throw IOException(
            __FILE__, __LINE__,
            "BufferedInputStream::available - Buffer was closed");
    }

    return buffer->available() + inputStream->available();
}

////////////////////////////////////////////////////////////////////////////////
void BufferedInputStream::mark( int readLimit ) {
    if( this->buffer != NULL ) {
        this->buffer->mark( readLimit );
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedInputStream::reset() throw ( IOException ) {

    if( this->isClosed() ) {
        throw IOException(
            __FILE__, __LINE__,
            "BufferedInputStream::reset - This stream has been closed." );
    }

    if( !this->buffer->isMarked() ) {
        throw IOException(
            __FILE__, __LINE__,
            "BufferedInputStream::reset - The mark position was invalidated." );
    }

    this->buffer->reset();
}

////////////////////////////////////////////////////////////////////////////////
int BufferedInputStream::doReadByte() throw ( IOException ){

    try{

        // Use a local reference in case of unsynchronized close.
        InputStream* inputStream = this->inputStream;

        if( isClosed() || inputStream == NULL ){
            throw IOException(
                __FILE__, __LINE__,
                "BufferedInputStream::bufferData - Stream is closed" );
        }

        // Are there buffered bytes available?  Or can we read more?
        if( this->buffer->isEmpty() && bufferData( inputStream ) == -1 ) {
            return -1;
        }

        // Stream might have closed while we were buffering.
        if( isClosed() ){
            throw IOException(
                __FILE__, __LINE__,
                "BufferedInputStream::bufferData - Stream is closed" );
        }

        if( !this->buffer->isEmpty() ) {
            return this->buffer->next();
        }

        return -1;
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCHALL_THROW( IOException )
}

////////////////////////////////////////////////////////////////////////////////
int BufferedInputStream::doReadArrayBounded( unsigned char* buffer, std::size_t size,
                                             std::size_t offset, std::size_t length )
    throw ( decaf::io::IOException,
            decaf::lang::exceptions::IndexOutOfBoundsException,
            decaf::lang::exceptions::NullPointerException ) {

    try{

        // Use a local reference in case of unsynchronized close.
        InputStream* inputStream = this->inputStream;

        if( isClosed() ){
            throw IOException(
                __FILE__, __LINE__, "Stream is closed" );
        }

        if( buffer == NULL ) {
            throw NullPointerException(
                __FILE__, __LINE__,
                "Buffer passed was NULL." );
        }

        if( offset + length > size ) {
            throw IndexOutOfBoundsException(
                __FILE__, __LINE__,
                "Given size{%d} - offset{%d} is less than length{%d}.", size, offset, length );
        }

        // For zero, do nothing
        if( length == 0 ) {
            return 0;
        }

        if( inputStream == NULL ){
            throw IOException(
                __FILE__, __LINE__, "Stream is closed" );
        }

        std::size_t required = 0;

        // There are bytes available in the buffer so use them up first and
        // then we check to see if any are available on the stream, if not
        // then just return what we had.
        if( !this->buffer->isEmpty() ) {

            std::size_t copylength =
                this->buffer->available() >= length ? length : this->buffer->available();

            System::arraycopy( this->buffer->getBuffer(), this->buffer->getPos(),
                               buffer, offset, copylength );
            this->buffer->advance( copylength );

            if( copylength == length || inputStream->available() == 0 ) {
                return copylength;
            }

            offset += copylength;
            required = length - copylength;
        } else {
            required = length;
        }

        while( true ) {

            int read = 0;

            // If we're not marked and the required size is greater than the
            // buffer, simply read the bytes directly bypassing the buffer.
            if( !this->buffer->isMarked() && required >= this->buffer->getBufferSize() ) {

                read = inputStream->read( buffer, size, offset, required );
                if( read == -1 ) {
                    return required == length ? -1 : length - required;
                }

            } else {

                if( bufferData( inputStream ) == -1 ) {
                    return required == length ? -1 : length - required;
                }

                // Stream might have closed while we were buffering.
                if( isClosed() ){
                    throw IOException(
                        __FILE__, __LINE__,
                        "BufferedInputStream::bufferData - Stream is closed" );
                }

                read = this->buffer->available() >= required ? required : this->buffer->available();
                System::arraycopy( this->buffer->getBuffer(), this->buffer->getPos(),
                                   buffer, offset, read );
                this->buffer->advance( read );
            }

            required -= read;

            if( required == 0 ) {
                return length;
            }

            if( inputStream->available() == 0 ) {
                return length - required;
            }

            offset += read;
        }
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCH_RETHROW( NullPointerException )
    DECAF_CATCH_RETHROW( IndexOutOfBoundsException )
    DECAF_CATCHALL_THROW( IOException )
}

////////////////////////////////////////////////////////////////////////////////
std::size_t BufferedInputStream::skip( std::size_t amount )
    throw ( IOException, lang::exceptions::UnsupportedOperationException ){

    try{

        if( amount == 0 ) {
            return 0;
        }

        // Use a local reference in case of unsynchronized close.
        InputStream* inputStream = this->inputStream;

        if( isClosed() || inputStream == NULL ){
            throw IOException(
                __FILE__, __LINE__,
                "BufferedInputStream::bufferData - Stream is closed" );
        }

        if( buffer->available() >= amount ) {
            buffer->advance( amount );
            return amount;
        }

        int read = buffer->available();

        buffer->advance( buffer->getCount() );

        if( buffer->isMarked() ) {

            if( amount <= buffer->getMarkLimit() ) {

                if( bufferData( inputStream ) == -1 ) {
                    return read;
                }

                if( buffer->available() >= ( amount - read ) ) {
                    buffer->advance( amount - read );
                    return amount;
                }

                // Couldn't get all the bytes, skip what we read
                read += buffer->available();
                buffer->advance( buffer->getCount() );

                return read;
            }
        }

        return read + inputStream->skip( amount - read );
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCHALL_THROW( IOException )
}

////////////////////////////////////////////////////////////////////////////////
int BufferedInputStream::bufferData( InputStream* inputStream ) throw ( IOException ){

    try{

        if( !buffer->isMarked() || buffer->isReadLimitExceeded() ) {
            // Mark position not set or exceeded readlimit
            int result = inputStream->read( buffer->getBuffer(), buffer->getBufferSize() );
            if( result > 0 ) {
                buffer->unmark();
                buffer->clear();
                buffer->advanceTail( result == -1 ? 0 : result );
            }

            return result;
        }

        std::size_t markPos = buffer->getMarkPos();
        std::size_t markLimit = buffer->getMarkLimit();

        if( markPos == 0 && markLimit > buffer->getBufferSize() ) {

            // Increase buffer size to accommodate the readlimit.
            std::size_t newLength = buffer->getBufferSize() * 2;
            if( newLength > markLimit ) {
                newLength = markLimit;
            }
            this->buffer->resize( newLength );
        } else if( markPos > 0 ) {
            System::arraycopy( buffer->getBuffer(), markPos,
                               buffer->getBuffer(), 0, buffer->getBufferSize() - markPos );
        }

        // Set the new position and mark position
        buffer->reverse( markPos );
        buffer->setCount( 0 );
        buffer->setMarkPos( 0 );

        int bytesread = inputStream->read( buffer->getBuffer(), buffer->getBufferSize(),
                                           buffer->getPos(), buffer->getBufferSize() - buffer->getPos() );

        buffer->setCount( bytesread <= 0 ? buffer->getPos() : buffer->getPos() + bytesread );

        return bytesread;
    }
    DECAF_CATCH_RETHROW( IOException )
    DECAF_CATCHALL_THROW( IOException )
}
