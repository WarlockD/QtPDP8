#include "wiretest.h"

namespace WireSim {

    void CircuitSimulatorBase::recalcWireList(const WireIndexList& nwireList)
    {
        prepForRecalc();
        for(index_t wireIndex : nwireList) {
            recalcOrder[lastRecalcOrder] = wireIndex;
            recalcArray[wireIndex] = true;
            lastRecalcOrder++;
        }
        doRecalcIterations();
    }
    void CircuitSimulatorBase::recalcNamedWire(const std::string& wireName)  {
        recalcWire(getWireIndex(wireName));
    }
    void CircuitSimulatorBase::prepForRecalc() {
        if(recalcArray.empty()) {
            recalcCap = _transistorList.size();
            // use lists, faster than arrays? humm
            recalcArray.clear(); recalcOrder.clear(); newRecalcOrder.clear(); newRecalcArray.clear();
            recalcArray.resize(recalcCap);recalcOrder.resize(recalcCap);newRecalcOrder.resize(recalcCap);newRecalcArray.resize(recalcCap);
        }
        newLastRecalcOrder = 0;
        lastRecalcOrder = 0;
    }

    void CircuitSimulatorBase::recalcWireNameList(const WireNameList& wireNameList) {
        WireIndexList ilist;
        ilist.reserve(wireNameList.size());
        for(const std::string& name : wireNameList) {
            ilist.push_back(getWireIndex(name)); // this will be so SO slow
        }
        recalcWireList(ilist);
    }
    void CircuitSimulatorBase::recalcAllWires() {
        WireIndexList ilist;
        for(size_t i=0;i < _wireList.size();i++)
            if(_wireList[i] != nullptr) ilist.push_back(i);
        recalcWireList(ilist);
    }

    void CircuitSimulatorBase::recalcWire(index_t wireIndex) {
        prepForRecalc();
        recalcOrder[lastRecalcOrder] = wireIndex;
        recalcArray[wireIndex] = true;
        lastRecalcOrder++;

        doRecalcIterations();
    }
    void CircuitSimulatorBase::doRecalcIterations() {
        // Simulation is not allowed to try more than 'stepLimit'
        // iterations.  If it doesn't converge by then, raise an
            // exception.
        size_t step = 0;
        size_t stepLimit = 400;
        while(step < stepLimit) {
            if(lastRecalcOrder==0) break;
            for(size_t i=0; i< lastRecalcOrder;i++) {
                index_t wireIndex = recalcOrder[i];
                newRecalcArray[wireIndex] = 0;
                doWireRecalc(wireIndex);
                recalcArray[wireIndex] = true;
                numWiresRecalculated++;
            }
            std::swap(recalcArray,newRecalcArray);
            std::swap(recalcOrder,newRecalcOrder);
            lastRecalcOrder = newLastRecalcOrder;
            newLastRecalcOrder = 0;
            step++;
        }
        if(step >= stepLimit) {
            std::stringstream msg;
            msg << "ERROR: Sim \"" << _name << "\" did not converage after " << stepLimit << " iterations";
            if(callback_addLogStr) callback_addLogStr(msg.str());
            // Don't raise an exception if this is the first attempt
            // to compute the state of a chip, but raise an exception if
            // the simulation doesn't converge any time other than that.
            if(halfClkCount>0) throw std::runtime_error(msg.str());
        }
        // Check that we've properly reset the recalcArray.  All entries
        // should be zero in preparation for the next half clock cycle.
        // Only do this sanity check for the first clock cycles.
        if(halfClkCount < 20) {
            bool needNewArray = false;
            for(bool recal : recalcArray) {
                if(recal) {
                    needNewArray = true;
                    if(step<stepLimit) {
                        std::stringstream msg;
                        msg << "ERROR: at halfclk " << halfClkCount << " after " << step << " iterations an entry in recalcArray is not False at the end of an update";
                        if(callback_addLogStr) callback_addLogStr(msg.str());
                        break;
                    }
                }
            }
            if(needNewArray) { std::fill(recalcArray.begin(), recalcArray.end(), false); }
        }

    }
}
