//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _publisher_h
#define _publisher_h

#include "baseIce.h"

#include <messages.h>

namespace SC {

class SCLIB_EXPORT Publisher: public BaseIce {
private:
    /*! Default constructor should not be used */
    Publisher(void);

protected:
    /*! typedef for base class */
    typedef BaseIce BaseType;

    /*! Publisher id (unique within a process) */
    static unsigned int Id;

    /*! IceStorm publisher proxy */
    DataPrx PublisherData;
    ControlPrx PublisherControl;

    /*! Initialization */
    void Init(void);

public:
    //! Constructor
    /*!
       Construct publisher objects with a topic name and a default property 
       file for publisher.
       \param topicName Name of topic
       \sa GetDefaultConfigFilePath()
    */
    Publisher(const std::string & topicName);

    //! Constructor
    /*!
        Construct publisher objects with a topic name and a user-specified 
        IceStorm property file.
        \param topicName Name of topic
        \param propertyFileName Path to publisher property file
    */
    Publisher(const std::string & topicName, const std::string & propertyFileName);
    
    //! Destructor
    virtual ~Publisher();

    //! Start up Ice publisher
    bool Startup(void);

    //! Publish messages to "Data" topic
    /*!
        Publish messages to "Data" topic with a given category.
        \param category Type of data message.  Defined in topic_def.h
        \param json Messages encoded in the JSON format
        \return true if success.  false, otherwise (e.g., invalid category or json string)
        \sa topic_def.h
    */
    bool PublishData(const Topic::Data::CategoryType category, const std::string & json);

    //! Publish messages to "Control" topic
    /*!
        Publish messages to "Control" topic with a given category.
        \param category Type of control message.  Defined in topic_def.h
        \param json Messages encoded in the JSON format
        \return true if success.  false, otherwise (e.g., invalid category or json string)
        \sa topic_def.h
    */
    bool PublishControl(const Topic::Control::CategoryType category, const std::string & json);
    
    //! Stop the publisher
    void Stop(void);

    //! Returns default publisher configuration file
    static const std::string GetDefaultConfigFilePath(void);
};

};

#endif // _publisher_h
