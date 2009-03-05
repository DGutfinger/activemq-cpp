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

#include <activemq/transport/mock/MockTransportFactory.h>
//#include <activemq/wireformat/stomp/StompResponseBuilder.h>
#include <activemq/wireformat/openwire/OpenWireResponseBuilder.h>
#include <activemq/transport/Transport.h>
#include <activemq/transport/mock/MockTransport.h>

using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::mock;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
Pointer<Transport> MockTransportFactory::doCreateComposite(
    const decaf::net::URI& location AMQCPP_UNUSED,
    const Pointer<wireformat::WireFormat>& wireFormat,
    const decaf::util::Properties& properties )
        throw ( exceptions::ActiveMQException ) {

    try {

        std::string wireFormatName =
            properties.getProperty( "wireFormat", "openwire" );

        Pointer<MockTransport::ResponseBuilder> builder;

        if( wireFormatName == "stomp" ) {
//            builder.reset( new wireformat::stomp::StompResponseBuilder() );
        } else if( wireFormatName == "openwire" ) {
            builder.reset( new wireformat::openwire::OpenWireResponseBuilder() );
        }

        return Pointer<Transport>( new MockTransport( wireFormat, builder ) );
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}
