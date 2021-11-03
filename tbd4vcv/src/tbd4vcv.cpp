#include "plugin.hpp"
#include "WebServer.hpp"
#include "SPManager.hpp"
#include <iostream>


struct tbd4vcv : rack::Module {
	enum ParamIds {
		BTN_TRIG_0_PARAM,
		BTN_TRIG_1_PARAM,
		POT0_PARAM,
		POT1_PARAM,
		GAIN0_PARAM,
		GAIN1_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN0_INPUT,
		IN1_INPUT,
		TRIG0_INPUT,
		TRIG1_INPUT,
		CV0_INPUT,
		CV1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT0_OUTPUT,
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BTN_TRIG_0_LIGHT,
		NUM_LIGHTS
	};

	tbd4vcv() {
        if(instanceCount == 0){
            server.Start(3000);
            activeServerInstance = this;
        }
        instanceCount++;
        std::cerr << "Instance number " << instanceCount << std::endl;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(BTN_TRIG_0_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BTN_TRIG_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configParam(POT1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GAIN0_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GAIN1_PARAM, 0.f, 1.f, 0.f, "");
	}
    ~tbd4vcv(){
        rack::logger::log(rack::Level::DEBUG_LEVEL, "tbd4vcv.cpp", 48, "Destructor called");
        instanceCount--;
        if(activeServerInstance == this) activeServerInstance = nullptr;
        if(instanceCount == 0){
            server.Stop();
        }
    }

	void process(const ProcessArgs& args) override {

	}

    static tbd4vcv* activeServerInstance;

private:
    static WebServer server;
    static int instanceCount;
    CTAG::AUDIO::SPManager spManager;
};

int tbd4vcv::instanceCount {0};
tbd4vcv* tbd4vcv::activeServerInstance {nullptr};
WebServer tbd4vcv::server;

struct tbd4vcvWidget : rack::ModuleWidget {
	tbd4vcvWidget(tbd4vcv* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/tbd4vcv.svg")));

		addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(rack::RACK_GRID_WIDTH, 0)));
		addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(box.size.x - 2 * rack::RACK_GRID_WIDTH, 0)));
		addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(rack::RACK_GRID_WIDTH, rack::RACK_GRID_HEIGHT - rack::RACK_GRID_WIDTH)));
		addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(box.size.x - 2 * rack::RACK_GRID_WIDTH, rack::RACK_GRID_HEIGHT - rack::RACK_GRID_WIDTH)));

		addParam(rack::createParamCentered<rack::BefacoPush>(rack::mm2px(rack::Vec(7.406, 49.539)), module, tbd4vcv::BTN_TRIG_0_PARAM));
		addParam(rack::createParamCentered<rack::BefacoPush>(rack::mm2px(rack::Vec(32.806, 49.539)), module, tbd4vcv::BTN_TRIG_1_PARAM));
		addParam(rack::createParamCentered<rack::Trimpot>(rack::mm2px(rack::Vec(7.406, 62.333)), module, tbd4vcv::POT0_PARAM));
		addParam(rack::createParamCentered<rack::Trimpot>(rack::mm2px(rack::Vec(32.806, 62.333)), module, tbd4vcv::POT1_PARAM));
		addParam(rack::createParamCentered<rack::Trimpot>(rack::mm2px(rack::Vec(7.406, 78.933)), module, tbd4vcv::GAIN0_PARAM));
		addParam(rack::createParamCentered<rack::Trimpot>(rack::mm2px(rack::Vec(32.806, 78.933)), module, tbd4vcv::GAIN1_PARAM));

		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(4.897, 96.524)), module, tbd4vcv::IN0_INPUT));
		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(15.057, 96.524)), module, tbd4vcv::IN1_INPUT));
		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(25.217, 96.524)), module, tbd4vcv::TRIG0_INPUT));
		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(35.377, 96.524)), module, tbd4vcv::TRIG1_INPUT));
		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(25.217, 109.478)), module, tbd4vcv::CV0_INPUT));
		addInput(rack::createInputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(35.377, 109.478)), module, tbd4vcv::CV1_INPUT));

		addOutput(rack::createOutputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(4.897, 109.478)), module, tbd4vcv::OUT0_OUTPUT));
		addOutput(rack::createOutputCentered<rack::PJ301MPort>(rack::mm2px(rack::Vec(15.057, 109.478)), module, tbd4vcv::OUT1_OUTPUT));

		addChild(rack::createLightCentered<rack::MediumLight<rack::RedLight>>(rack::mm2px(rack::Vec(20.123, 57.802)), module, tbd4vcv::BTN_TRIG_0_LIGHT));
	}

    void appendContextMenu(rack::Menu* menu) override {
        rack::logger::log(rack::Level::DEBUG_LEVEL, "tbd4vcv.cpp", 98, "appendContextMenu called");
        tbd4vcv* module = dynamic_cast<tbd4vcv*>(this->module);

        menu->addChild(new rack::MenuEntry);
        menu->addChild(rack::createMenuLabel("Enable Web Server"));

        // happens when action is performed on view
        struct ServerItem : rack::MenuItem {
            tbd4vcv* module;
            void onAction(const rack::event::Action& e) override {
                if(module->activeServerInstance == module){
                    module->activeServerInstance = nullptr;
                }else{
                    module->activeServerInstance = module;
                }
            }
        };

        // happens when view is rendered
        std::string serverItemName = {"Active"};
        ServerItem* serverItem = rack::createMenuItem<ServerItem>(serverItemName);
        serverItem->rightText = CHECKMARK(module == module->activeServerInstance);
        serverItem->module = module;
        menu->addChild(serverItem);
    }
};


rack::Model* modeltbd4vcv = rack::createModel<tbd4vcv, tbd4vcvWidget>("tbd4vcv");