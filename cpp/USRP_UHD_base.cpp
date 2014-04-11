
#include "USRP_UHD_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the device class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

USRP_UHD_base::USRP_UHD_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
    frontend::FrontendTunerDevice<frontend_tuner_status_struct_struct>(devMgr_ior, id, lbl, sftwrPrfl),
    serviceThread(0)
{
}

USRP_UHD_base::USRP_UHD_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
    frontend::FrontendTunerDevice<frontend_tuner_status_struct_struct>(devMgr_ior, id, lbl, sftwrPrfl, compDev),
    serviceThread(0)
{
}

USRP_UHD_base::USRP_UHD_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
    frontend::FrontendTunerDevice<frontend_tuner_status_struct_struct>(devMgr_ior, id, lbl, sftwrPrfl, capacities),
    serviceThread(0)
{
}

USRP_UHD_base::USRP_UHD_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
    frontend::FrontendTunerDevice<frontend_tuner_status_struct_struct>(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev),
    serviceThread(0)
{
}

void USRP_UHD_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    RFInfo_in = new frontend::InRFInfoPort("RFInfo_in", this);

    oid = ossie::corba::RootPOA()->activate_object(RFInfo_in);
    DigitalTuner_in = new frontend::InDigitalTunerPort("DigitalTuner_in", this);

    oid = ossie::corba::RootPOA()->activate_object(DigitalTuner_in);
    dataShortTX_in = new bulkio::InShortPort("dataShortTX_in");

    oid = ossie::corba::RootPOA()->activate_object(dataShortTX_in);
    dataFloatTX_in = new bulkio::InFloatPort("dataFloatTX_in");

    oid = ossie::corba::RootPOA()->activate_object(dataFloatTX_in);
    dataShort_out = new bulkio::OutShortPort("dataShort_out");

    oid = ossie::corba::RootPOA()->activate_object(dataShort_out);
    RFInfoTX_out = new frontend::OutRFInfoPort("RFInfoTX_out");

    oid = ossie::corba::RootPOA()->activate_object(RFInfoTX_out);

    registerInPort(RFInfo_in);
    registerInPort(DigitalTuner_in);
    registerInPort(dataShortTX_in);
    registerInPort(dataFloatTX_in);
    registerOutPort(dataShort_out, dataShort_out->_this());
    registerOutPort(RFInfoTX_out, RFInfoTX_out->_this());
    this->addPropertyChangeListener("connectionTable",this,&USRP_UHD_base::connectionTableChanged);
}

/* This sets the number of entries in the frontend_tuner_status struct sequence property
 *  * as well as the tuner_allocation_ids vector. Call this function during initialization
 *   */
void USRP_UHD_base::setNumChannels(size_t num)
{
    frontend_tuner_status.clear();
    frontend_tuner_status.resize(num);
    tuner_allocation_ids.clear();
    tuner_allocation_ids.resize(num);
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void USRP_UHD_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void USRP_UHD_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        dataShortTX_in->unblock();
        dataFloatTX_in->unblock();
        serviceThread = new ProcessThread<USRP_UHD_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
        Resource_impl::start();
    }
}

void USRP_UHD_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        dataShortTX_in->block();
        dataFloatTX_in->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
        Resource_impl::stop();
    }
}

CORBA::Object_ptr USRP_UHD_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {
        if (!strcmp(_id,"RFInfo_in")) {
            frontend::InRFInfoPort *ptr = dynamic_cast<frontend::InRFInfoPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"DigitalTuner_in")) {
            frontend::InDigitalTunerPort *ptr = dynamic_cast<frontend::InDigitalTunerPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataShortTX_in")) {
            bulkio::InShortPort *ptr = dynamic_cast<bulkio::InShortPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataFloatTX_in")) {
            bulkio::InFloatPort *ptr = dynamic_cast<bulkio::InFloatPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void USRP_UHD_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the device running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    // deactivate ports
    releaseInPorts();
    releaseOutPorts();

    delete(RFInfo_in);
    delete(DigitalTuner_in);
    delete(dataShortTX_in);
    delete(dataFloatTX_in);
    delete(dataShort_out);
    delete(RFInfoTX_out);

    Device_impl::releaseObject();
}

void USRP_UHD_base::loadProperties()
{
    addProperty(device_kind,
                "FRONTEND::TUNER",
                "DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d",
                "device_kind",
                "readonly",
                "",
                "eq",
                "allocation,configure");

    addProperty(device_model,
                "DCE:0f99b2e4-9903-4631-9846-ff349d18ecfb",
                "device_model",
                "readonly",
                "",
                "eq",
                "allocation,configure");

    addProperty(update_available_devices,
                false,
                "update_available_devices",
                "update_available_devices",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(device_ip_address,
                "device_ip_address",
                "device_ip_address",
                "readwrite",
                "",
                "external",
                "execparam,configure");

    addProperty(device_reference_source_global,
                "INTERNAL",
                "device_reference_source_global",
                "device_reference_source_global",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(device_gain_global,
                0,
                "device_gain_global",
                "device_gain_global",
                "readwrite",
                "dB",
                "external",
                "configure");

    addProperty(device_group_id_global,
                "device_group_id_global",
                "device_group_id_global",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(frontend_listener_allocation,
                frontend::frontend_listener_allocation_struct(),
                "FRONTEND::listener_allocation",
                "frontend_listener_allocation",
                "readwrite",
                "",
                "external",
                "allocation");

    addProperty(frontend_tuner_allocation,
                frontend::frontend_tuner_allocation_struct(),
                "FRONTEND::tuner_allocation",
                "frontend_tuner_allocation",
                "readwrite",
                "",
                "external",
                "allocation");

    addProperty(frontend_tuner_status,
                "FRONTEND::tuner_status",
                "frontend_tuner_status",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(connectionTable,
                "connectionTable",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(available_devices,
                "available_devices",
                "available_devices",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(device_channels,
                "device_channels",
                "device_channels",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(device_motherboards,
                "device_motherboards",
                "device_motherboards",
                "readonly",
                "",
                "external",
                "configure");

}

std::string USRP_UHD_base::get_rf_flow_id(const std::string& port_name){return std::string("none");}
void USRP_UHD_base::set_rf_flow_id(const std::string& port_name, const std::string& id){}
frontend::RFInfoPkt USRP_UHD_base::get_rfinfo_pkt(const std::string& port_name){frontend::RFInfoPkt tmp;return tmp;}
void USRP_UHD_base::set_rfinfo_pkt(const std::string& port_name, const frontend::RFInfoPkt &pkt){}
void USRP_UHD_base::frontendTunerStatusChanged(const std::vector<frontend_tuner_status_struct_struct>* oldValue, const std::vector<frontend_tuner_status_struct_struct>* newValue)
{
    this->tuner_allocation_ids.resize(this->frontend_tuner_status.size());
}

CF::Properties* USRP_UHD_base::getTunerStatus(const std::string& allocation_id)
{
    CF::Properties* tmpVal = new CF::Properties();
    long tuner_id = getTunerMapping(allocation_id);
    if (tuner_id < 0)
        throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(allocation_id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
    CORBA::Any prop;
    prop <<= *(static_cast<frontend_tuner_status_struct_struct*>(&this->frontend_tuner_status[tuner_id]));
    prop >>= tmpVal;

    CF::Properties_var tmp = new CF::Properties(*tmpVal);
    return tmp._retn();
}

void USRP_UHD_base::assignListener(const std::string& listen_alloc_id, const std::string& allocation_id)
{
    listeners[listen_alloc_id] = allocation_id;
    std::vector<connection_descriptor_struct> old_table = this->connectionTable;
    for (std::map<std::string, std::string>::iterator listener=listeners.begin();listener!=listeners.end();listener++) {
        std::vector<std::string> streamids;
        std::vector<std::string> port_names;
        for (std::vector<connection_descriptor_struct>::iterator entry=this->connectionTable.begin();entry!=this->connectionTable.end();entry++) {
            if (entry->connection_id == listener->second) {
                streamids.push_back(entry->stream_id);
                port_names.push_back(entry->port_name);
            }
        }
        for (unsigned int i=0; i<streamids.size(); i++) {
            bool foundEntry = false;
            for (std::vector<connection_descriptor_struct>::iterator entry=this->connectionTable.begin();entry!=this->connectionTable.end();entry++) {
                if ((entry->stream_id == streamids[i]) and (entry->connection_id == listen_alloc_id)) {
                    foundEntry = true;
                    break;
                }
            }
            if (!foundEntry) {
                connection_descriptor_struct tmp;
                tmp.stream_id = streamids[i];
                tmp.connection_id = listen_alloc_id;
                tmp.port_name = port_names[i];
                this->connectionTable.push_back(tmp);
            }
        }
    }
    this->connectionTableChanged(&old_table, &this->connectionTable);
}

void USRP_UHD_base::removeListener(const std::string& listen_alloc_id)
{
    std::vector<connection_descriptor_struct> old_table = this->connectionTable;
    if (listeners.find(listen_alloc_id) != listeners.end()) {
        listeners.erase(listen_alloc_id);
    }
    std::vector<connection_descriptor_struct>::iterator entry = this->connectionTable.begin();
    while (entry != this->connectionTable.end()) {
        if (entry->connection_id == listen_alloc_id) {
            entry = this->connectionTable.erase(entry);
        } else {
            entry++;
        }
    }
    ExtendedCF::UsesConnectionSequence_var tmp;
    // Check to see if port "dataShort_out" is on connectionTable (old or new)
    tmp = this->dataShort_out->connections();
    for (unsigned int i=0; i<this->dataShort_out->connections()->length(); i++) {
        std::string connection_id = ossie::corba::returnString(tmp[i].connectionId);
        if (connection_id == listen_alloc_id) {
            this->dataShort_out->disconnectPort(connection_id.c_str());
        }
    }
    this->connectionTableChanged(&old_table, &this->connectionTable);
}
 
void USRP_UHD_base::setTunerCenterFrequency(const std::string& allocation_id, double freq){throw FRONTEND::NotSupportedException("setTunerCenterFrequency not supported");}
double USRP_UHD_base::getTunerCenterFrequency(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerCenterFrequency not supported");return 0.0;}
void USRP_UHD_base::setTunerBandwidth(const std::string& allocation_id, double bw){throw FRONTEND::NotSupportedException("setTunerBandwidth not supported");}
double USRP_UHD_base::getTunerBandwidth(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerBandwidth not supported");return 0.0;}
void USRP_UHD_base::setTunerAgcEnable(const std::string& allocation_id, bool enable){throw FRONTEND::NotSupportedException("setTunerAgcEnable not supported");}
bool USRP_UHD_base::getTunerAgcEnable(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerAgcEnable not supported");return false;}
void USRP_UHD_base::setTunerGain(const std::string& allocation_id, float gain){throw FRONTEND::NotSupportedException("setTunerGain not supported");}
float USRP_UHD_base::getTunerGain(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerGain not supported");return 0.0;}
void USRP_UHD_base::setTunerReferenceSource(const std::string& allocation_id, long source){throw FRONTEND::NotSupportedException("setTunerReferenceSource not supported");}
long USRP_UHD_base::getTunerReferenceSource(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerReferenceSource not supported");return 0;}
void USRP_UHD_base::setTunerEnable(const std::string& allocation_id, bool enable){throw FRONTEND::NotSupportedException("setTunerEnable not supported");}
bool USRP_UHD_base::getTunerEnable(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerEnable not supported");return false;}
void USRP_UHD_base::setTunerOutputSampleRate(const std::string& allocation_id, double sr){throw FRONTEND::NotSupportedException("setTunerOutputSampleRate not supported");}
double USRP_UHD_base::getTunerOutputSampleRate(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerOutputSampleRate not supported");return 0.0;}
std::string USRP_UHD_base::getTunerType(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerType not supported");return std::string("none");}
bool USRP_UHD_base::getTunerDeviceControl(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerDeviceControl not supported");return false;}
std::string USRP_UHD_base::getTunerGroupId(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerGroupId not supported");return std::string("none");}
std::string USRP_UHD_base::getTunerRfFlowId(const std::string& allocation_id){throw FRONTEND::NotSupportedException("getTunerRfFlowId not supported");return std::string("none");}
void USRP_UHD_base::connectionTableChanged(const std::vector<connection_descriptor_struct>* oldValue, const std::vector<connection_descriptor_struct>* newValue)
{
    // Check to see if port "dataShort_out" is on connectionTable (old or new)
    for (std::vector<connection_descriptor_struct>::const_iterator prop_itr = oldValue->begin(); prop_itr != oldValue->end(); prop_itr++) {
        if (prop_itr->port_name == "dataShort_out") {
            dataShort_out->updateConnectionFilter(*newValue);
            break;
        }
    }
    for (std::vector<connection_descriptor_struct>::const_iterator prop_itr = newValue->begin(); prop_itr != newValue->end(); prop_itr++) {
        if (prop_itr->port_name == "dataShort_out") {
            dataShort_out->updateConnectionFilter(*newValue);
            break;
        }
    }
    }
void USRP_UHD_base::matchAllocationIdToStreamId(const std::string allocation_id, const std::string stream_id, const std::string port_name) {
    if (port_name != "") {
        for (std::vector<connection_descriptor_struct>::iterator prop_itr = this->connectionTable.begin(); prop_itr!=this->connectionTable.end(); prop_itr++) {
            if ((*prop_itr).port_name != port_name)
                continue;
            if ((*prop_itr).stream_id != stream_id)
                continue;
            if ((*prop_itr).connection_id != allocation_id)
                continue;
            // all three match. This is a repeat
            return;
        }
        std::vector<connection_descriptor_struct> old_table = this->connectionTable;
        connection_descriptor_struct tmp;
        tmp.connection_id = allocation_id;
        tmp.port_name = port_name;
        tmp.stream_id = stream_id;
        this->connectionTable.push_back(tmp);
        this->connectionTableChanged(&old_table, &this->connectionTable);
        return;
    }
    std::vector<connection_descriptor_struct> old_table = this->connectionTable;
    connection_descriptor_struct tmp;
    tmp.connection_id = allocation_id;
    tmp.port_name = "dataShort_out";
    tmp.stream_id = stream_id;
    this->connectionTable.push_back(tmp);
    this->connectionTableChanged(&old_table, &this->connectionTable);
}

void USRP_UHD_base::removeAllocationIdRouting(const size_t tuner_id) {
    std::string allocation_id = getControlAllocationId(tuner_id);
    std::vector<connection_descriptor_struct> old_table = this->connectionTable;
    std::vector<connection_descriptor_struct>::iterator itr = this->connectionTable.begin();
    while (itr != this->connectionTable.end()) {
        if (itr->connection_id == allocation_id) {
            itr = this->connectionTable.erase(itr);
            continue;
        }
        itr++;
    }
    for (std::map<std::string, std::string>::iterator listener=listeners.begin();listener!=listeners.end();listener++) {
        if (listener->second == allocation_id) {
            std::vector<connection_descriptor_struct>::iterator itr = this->connectionTable.begin();
            while (itr != this->connectionTable.end()) {
                if (itr->connection_id == listener->first) {
                    itr = this->connectionTable.erase(itr);
                    continue;
                }
                itr++;
            }
        }
    }
    this->connectionTableChanged(&old_table, &this->connectionTable);
}

void USRP_UHD_base::removeStreamIdRouting(const std::string stream_id, const std::string allocation_id) {
    std::vector<connection_descriptor_struct> old_table = this->connectionTable;
    std::vector<connection_descriptor_struct>::iterator itr = this->connectionTable.begin();
    while (itr != this->connectionTable.end()) {
        if (allocation_id == "") {
            if (itr->stream_id == stream_id) {
                itr = this->connectionTable.erase(itr);
                continue;
            }
        } else {
            if ((itr->stream_id == stream_id) and (itr->connection_id == allocation_id)) {
                itr = this->connectionTable.erase(itr);
                continue;
            }
        }
        itr++;
    }
    for (std::map<std::string, std::string>::iterator listener=listeners.begin();listener!=listeners.end();listener++) {
        if (listener->second == allocation_id) {
            std::vector<connection_descriptor_struct>::iterator itr = this->connectionTable.begin();
            while (itr != this->connectionTable.end()) {
                if ((itr->connection_id == listener->first) and (itr->stream_id == stream_id)) {
                    itr = this->connectionTable.erase(itr);
                    continue;
                }
                itr++;
            }
        }
    }
    this->connectionTableChanged(&old_table, &this->connectionTable);
}


