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

#ifndef _DECAF_INTERNAL_IO_STANDARDINPUTSTREAM_H_
#define _DECAF_INTERNAL_IO_STANDARDINPUTSTREAM_H_

#include <decaf/util/Config.h>
#include <decaf/io/InputStream.h>

namespace decaf {
namespace internal {
namespace io {

    class DECAF_API StandardInputStream : public decaf::io::InputStream {
    public:

        StandardInputStream();

        virtual ~StandardInputStream();

        /**
         * Indicates the number of bytes available.
         * @return The number of bytes until the end of the internal buffer.
         */
        virtual std::size_t available() const throw ( decaf::io::IOException );

    protected:

        virtual int doReadByte() throw ( decaf::io::IOException );

    };

}}}

#endif /* _DECAF_INTERNAL_IO_STANDARDINPUTSTREAM_H_ */
