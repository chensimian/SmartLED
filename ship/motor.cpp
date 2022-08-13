#include <string>
#include <iostream>
#include <atomic>
#include <mqtt/async_client.h>

#include "socket.h"
#include "motor.h"

extern std::atomic_bool MOTOR_EXIT_FLAG;

const std::string TOPIC { "/diff" };
const int  mqttQOS = 1;
const auto TIMEOUT = std::chrono::seconds(10);

MotorData motorData;

class callback : public virtual mqtt::callback
{
public:
    void connection_lost(const std::string& cause) override {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override {
        //std::cout << "\tDelivery complete for token: "
            //<< (tok ? tok->get_message_id() : -1) << std::endl;
    }
};

class action_listener : public virtual mqtt::iaction_listener
{
protected:
    void on_failure(const mqtt::token& tok) override {
        std::cout << "\tListener failure for token: "
            << tok.get_message_id() << std::endl;
    }

    void on_success(const mqtt::token& tok) override {
        std::cout << "\tListener success for token: "
            << tok.get_message_id() << std::endl;
    }
};

class delivery_action_listener : public action_listener
{
    std::atomic<bool> done_;

    void on_failure(const mqtt::token& tok) override {
        action_listener::on_failure(tok);
        done_ = true;
    }

    void on_success(const mqtt::token& tok) override {
        action_listener::on_success(tok);
        done_ = true;
    }

public:
    delivery_action_listener() : done_(false) {}
    bool is_done() const { return done_; }
};

int _thread_motor_updater(const std::string &address)
{
    /*
    while(1)
    {
        if(MOTOR_EXIT_FLAG) break;
        //std::cout<<"\t\t\t"<<motorData.leftPercent<<" "<<motorData.rightPercent<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return 0;
    */
    std::cout << "Initializing for server '" << address << "'..." << std::endl;
    mqtt::async_client client(address, "motor");

    callback cb;
    client.set_callback(cb);

    mqtt::connect_options conopts;

    std::cout << "  ...OK" << std::endl;

    try {
        std::cout << "\nConnecting..." << std::endl;
        mqtt::token_ptr conntok = client.connect(conopts);
        std::cout << "Waiting for the connection..." << std::endl;
        conntok->wait();

        std::cout << "\nSending next message..." << std::endl;
        mqtt::delivery_token_ptr pubtok;

        while(1)
        {
            if(MOTOR_EXIT_FLAG) break;
            diffdata d;
            d.left = motorData.leftPercent;
            d.right = motorData.rightPercent;
            d.priority = 1;
            std::cout<<"\t\t\t"<<d.left<<" "<<d.right<<std::endl;
            std::string payload = MotorData::makeDiffPayload(d);

            pubtok = client.publish(TOPIC, payload.data(), payload.size(), mqttQOS, false);
            /*
            std::cout << "  ...with token: " << pubtok->get_message_id() << std::endl;
            std::cout << "  ...for message with " << pubtok->get_message()->get_payload().size()
                << " bytes" << std::endl;
            */
            pubtok->wait_for(TIMEOUT);
            //std::cout << "  ...OK" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Double check that there are no pending tokens

        auto toks = client.get_pending_delivery_tokens();
        if (!toks.empty())
            std::cout << "Error: There are pending delivery tokens!" << std::endl;

        // Disconnect
        std::cout << "\nDisconnecting..." << std::endl;
        conntok = client.disconnect();
        conntok->wait();
        std::cout << "  ...OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

     return 0;
}
