#ifndef WIRETEST_H
#define WIRETEST_H
#include "includes.h"


namespace WireSim {
typedef uint64_t index_t; // 32 or 64?
typedef uint8_t state_t; // 32 or 64?
    class Wire {
    public: // make this an enum class?
       static const state_t PULLED_HIGH    = 1 << 0;
       static const state_t PULLED_LOW     = 1 << 1;
       static const state_t GROUNDED       = 1 << 2;
       static const state_t HIGH           = 1 << 3;
       static const state_t FLOATING_HIGH  = 1 << 4;
       static const state_t FLOATING_LOW   = 1 << 5;
       static const state_t FLOATING       = 1 << 6;
    public:
       Wire(index_t idIdex,const std::string& name, index_t controlTransIndices,index_t transGateIndices,uint64_t pulled){
           _index = idIdex;
           _name = name;
           _ctInds = controlTransIndices;
           _gateInds = transGateIndices;
           _pulled = pulled;
           _state = pulled;
       }
       std::string toString() const {
           std::stringstream ret;
           ret <<  "Wire \"" << _index <<  "\"" << _name << "': " << _state;
           return std::move(ret.str());
       }
       void setHigh() { _pulled = _state = PULLED_HIGH; }
       void setLow() { _pulled = _state = PULLED_LOW; }
       void setPulledHighOrLow(bool high) { _pulled = _state = (high ? PULLED_HIGH : PULLED_LOW); }
       bool isHigh() const { return _state == FLOATING_HIGH || _state == PULLED_HIGH  || _state == HIGH ; }
       bool isLow() const { return _state == FLOATING_LOW || _state == PULLED_LOW  || _state == GROUNDED ; }
       state_t state() const { return _state; }
       state_t pulled() const { return _pulled; }
       state_t setState(state_t value) { _state = value; }
       state_t setPulled(state_t value) { _pulled = value; }
    private:
       std::string _name;
       uint64_t _index;
       uint64_t _ctInds;
       uint64_t _gateInds;
       uint64_t _pulled;
       uint64_t _state;

    };
    typedef std::vector<std::string>  WireNameList;
    typedef std::vector<Wire> WireList;
    typedef std::vector<index_t> WireIndexList;
    class CircuitSimulatorBase {
        std::string _name;
        std::vector<Wire*> _wireList;
        std::vector<Wire*> _transistorList;
       // std::vector<Wire*> _recalcWireList;
        std::map<std::string,index_t> wireNames;  /// switch to unorderd_map once I get a string library up
        size_t halfClkCount;
        std::vector<bool> recalcArray;
        std::vector<bool> newRecalcArray;
        std::vector<index_t> recalcOrder;
        std::vector<index_t> newRecalcOrder;

        size_t newLastRecalcOrder;
        size_t lastRecalcOrder;
        size_t recalcCap;
        size_t numAddWireToGroup;
        size_t numAddWireTransistor;
        size_t numWiresRecalculated;
        std::function<void(const std::string&s)> callback_addLogStr;
    public:
        CircuitSimulatorBase() {
            halfClkCount=0;
            numAddWireToGroup=0;
            numAddWireTransistor=0;
            numWiresRecalculated=0;
        }
        void clearSimStats() {
            numAddWireToGroup = 0;
            numAddWireTransistor = 0;
        }
        index_t getWireIndex(const std::string& wireName) const {
            auto it = wireNames.find(wireName);
            if(it == wireNames.cend()) throw std::runtime_error("Wire name not found!");
            return it->second;
        }
         void recalcWireList(const WireIndexList& nwireList);
        void recalcNamedWire(const std::string& wireName);
        void prepForRecalc();
        void recalcWireNameList(const WireNameList& wireNameList);
        void recalcAllWires();
        void recalcWire(index_t wireIndex);
        void doRecalcIterations();
        void floatWire(index_t n) {
            Wire* wire = _wireList[n];
            if(wire->pulled() == Wire::PULLED_HIGH) wire->setState(Wire::PULLED_HIGH);
            else if(wire->pulled() == Wire::PULLED_LOW) wire->setState(Wire::PULLED_LOW);
            else {
                state_t state = wire->state();
                if(state == Wire::GROUNDED || state == Wire::PULLED_LOW) wire->setState(Wire::FLOATING_LOW);
                if(state == Wire::HIGH || state == Wire::PULLED_HIGH) wire->setState(Wire::FLOATING_HIGH);
            }
        }
        // setHighWN() and setLowWN() do not trigger an update
        // of the simulation.
        void setHighWN(index_t n) {
            Wire* wire = _wireList[n];
            if(!wire) std::runtime_error("Tring to set a null wire high");
            wire->setHigh();
        }
        void setHighWN(const std::string& n) { setHighWN(getWireIndex(n)); }
        void setLowWN(index_t n) {
            Wire* wire = _wireList[n];
            if(!wire) std::runtime_error("Tring to set a null wire high");
            wire->setLow();
        }
        void setLowWN(const std::string& n) { setHighWN(getWireIndex(n)); }
        void setHigh(index_t n) { _wireList[n]->setPulledHighOrLow(true); }
        void setLow(index_t n) { _wireList[n]->setPulledHighOrLow(false); }
        void setPulled(index_t n,bool value) { _wireList[n]->setPulledHighOrLow(value); }
        void setPulledHigh(index_t n) { _wireList[n]->setPulledHighOrLow(true); }
        void setPulledLow(index_t n) { _wireList[n]->setPulledHighOrLow(false); }
        bool isLow(index_t n) const { return _wireList[n]->isLow(); }
        bool isHigh(index_t n) const { return _wireList[n]->isHigh(); }


    public: // abstract classes
        virtual void doWireRecalc(index_t wireIndex) =0;
        virtual void turnTransistorOn(index_t t) =0;
        virtual void turnTransistorOff(index_t t) =0;

    };


}
class wiretest
{
public:
    wiretest();

signals:

public slots:
};

#endif // WIRETEST_H
