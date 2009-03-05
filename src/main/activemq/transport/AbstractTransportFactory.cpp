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

#include "AbstractTransportFactory.h"

#include <memory>
#include <string>
#include <activemq/wireformat/WireFormat.h>
#include <activemq/wireformat/WireFormatRegistry.h>
#include <activemq/util/URISupport.h>
#include <activemq/transport/correlator/ResponseCorrelator.h>
#include <activemq/transport/logging/LoggingTransport.h>

using namespace std;
using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::correlator;
using namespace activemq::transport::logging;
using namespace activemq::wireformat;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::net;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
Pointer<Transport> AbstractTransportFactory::create( const decaf::net::URI& location )
    throw ( exceptions::ActiveMQException ) {

    try{

        Properties properties =
            activemq::util::URISupport::parseQuery( location.getQuery() );

        Pointer<WireFormat> wireFormat = this->createWireFormat( properties );

        // Create the initial Transport, then wrap it in the normal Filters
        Pointer<Transport> transport( doCreateComposite( location, wireFormat, properties ) );

        // Create the Transport for response correlator
        transport.reset( new ResponseCorrelator( transport ) );

        // If command tracing was enabled, wrap the transport with a logging transport.
        if( properties.getProperty( "transport.commandTracingEnabled", "false" ) == "true" ) {
            // Create the Transport for response correlator
            transport.reset( new LoggingTransport( transport ) );
        }

        // If there is a negotiator need then we create and wrap here.
        if( wireFormat->hasNegotiator() ) {
            transport = wireFormat->createNegotiator( transport );
        }

        return transport;
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}

////////////////////////////////////////////////////////////////////////////////
Pointer<Transport> AbstractTransportFactory::createComposite( const decaf::net::URI& location )
    throw ( exceptions::ActiveMQException ) {

    try{

        Properties properties =
            activemq::util::URISupport::parseQuery( location.getQuery() );

        Pointer<WireFormat> wireFormat = this->createWireFormat( properties );

        // Create the initial Transport, then wrap it in the normal Filters
        Pointer<Transport> transport( doCreateComposite( location, wireFormat, properties ) );

        return transport;
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}

////////////////////////////////////////////////////////////////////////////////
Pointer<WireFormat> AbstractTransportFactory::createWireFormat(
    const decaf::util::Properties& properties )
        throw( decaf::lang::exceptions::NoSuchElementException ) {

    try{

        std::string wireFormat = properties.getProperty( "wireFormat", "openwire" );

        WireFormatFactory* factory =
            WireFormatRegistry::getInstance().findFactory( wireFormat );

        return Pointer<WireFormat>( factory->createWireFormat( properties ) );
    }
    AMQ_CATCH_RETHROW( NoSuchElementException )
    AMQ_CATCH_EXCEPTION_CONVERT( Exception, NoSuchElementException )
    AMQ_CATCHALL_THROW( NoSuchElementException )
}
