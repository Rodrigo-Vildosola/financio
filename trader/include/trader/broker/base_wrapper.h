#pragma once

// IB API
#include <twsapi/EWrapper.h>              // brings in forward decls & typedefs used below

// No-op base that satisfies all pure virtual methods in EWrapper.
// Derive your real wrapper from this and override only what you need.
class BaseEWrapper : public EWrapper {
public:
    ~BaseEWrapper() override = default;

    // ---- Market data ----
    void tickPrice(TickerId, TickType, double, const TickAttrib&) override {}
    void tickSize(TickerId, TickType, Decimal) override {}
    void tickOptionComputation(TickerId, TickType, int, double, double, double, double, double, double, double, double) override {}
    void tickGeneric(TickerId, TickType, double) override {}
    void tickString(TickerId, TickType, const std::string&) override {}
    void tickEFP(TickerId, TickType, double, const std::string&, double, int, const std::string&, double, double) override {}
    void tickSnapshotEnd(int) override {}
    void marketDataType(TickerId, int) override {}
    void tickReqParams(int, double, const std::string&, int) override {}

    // ---- Orders / execution ----
    void orderStatus(OrderId, const std::string&, Decimal, Decimal, double, long long, int, double, int, const std::string&, double) override {}
    void openOrder(OrderId, const Contract&, const Order&, const OrderState&) override {}
    void openOrderEnd() override {}
    void completedOrder(const Contract&, const Order&, const OrderState&) override {}
    void completedOrdersEnd() override {}
    void orderBound(long long, int, int) override {}
    void execDetails(int, const Contract&, const Execution&) override {}
    void execDetailsEnd(int) override {}

    // ---- Connection / errors ----
    void winError(const std::string&, int) override {}
    void connectionClosed() override {}
    void nextValidId(OrderId) override {}
    void error(int, time_t, int, const std::string&, const std::string&) override {}
    void connectAck() override {}

    // ---- Account / portfolio ----
    void updateAccountValue(const std::string&, const std::string&, const std::string&, const std::string&) override {}
    void updatePortfolio(const Contract&, Decimal, double, double, double, double, double, const std::string&) override {}
    void updateAccountTime(const std::string&) override {}
    void accountDownloadEnd(const std::string&) override {}
    void position(const std::string&, const Contract&, Decimal, double) override {}
    void positionEnd() override {}
    void positionMulti(int, const std::string&, const std::string&, const Contract&, Decimal, double) override {}
    void positionMultiEnd(int) override {}
    void accountUpdateMulti(int, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&) override {}
    void accountUpdateMultiEnd(int) override {}
    void managedAccounts(const std::string&) override {}
    void accountSummary(int, const std::string&, const std::string&, const std::string&, const std::string&) override {}
    void accountSummaryEnd(int) override {}

    // ---- Contract details / symbols ----
    void contractDetails(int, const ContractDetails&) override {}
    void bondContractDetails(int, const ContractDetails&) override {}
    void contractDetailsEnd(int) override {}
    void securityDefinitionOptionalParameter(int, const std::string&, int, const std::string&, const std::string&, const std::set<std::string>&, const std::set<double>&) override {}
    void securityDefinitionOptionalParameterEnd(int) override {}
    void familyCodes(const std::vector<FamilyCode>&) override {}
    void symbolSamples(int, const std::vector<ContractDescription>&) override {}
    void mktDepthExchanges(const std::vector<DepthMktDataDescription>&) override {}

    // ---- Depth ----
    void updateMktDepth(TickerId, int, int, int, double, Decimal) override {}
    void updateMktDepthL2(TickerId, int, const std::string&, int, int, double, Decimal, bool) override {}

    // ---- News ----
    void updateNewsBulletin(int, int, const std::string&, const std::string&) override {}
    void newsProviders(const std::vector<NewsProvider>&) override {}
    void newsArticle(int, int, const std::string&) override {}
    void historicalNews(int, const std::string&, const std::string&, const std::string&, const std::string&) override {}
    void historicalNewsEnd(int, bool) override {}
    void tickNews(int, time_t, const std::string&, const std::string&, const std::string&, const std::string&) override {}

    // ---- Scanners ----
    void scannerParameters(const std::string&) override {}
    void scannerData(int, int, const ContractDetails&, const std::string&, const std::string&, const std::string&, const std::string&) override {}
    void scannerDataEnd(int) override {}

    // ---- Real-time bars ----
    void realtimeBar(TickerId, long, double, double, double, double, Decimal, Decimal, int) override {}

    // ---- Fundamentals ----
    void fundamentalData(TickerId, const std::string&) override {}

    // ---- Delta neutral ----
    void deltaNeutralValidation(int, const DeltaNeutralContract&) override {}

    // ---- Time ----
    void currentTime(long) override {}
    void currentTimeInMillis(time_t) override {}

    // ---- Soft dollars ----
    void softDollarTiers(int, const std::vector<SoftDollarTier>&) override {}

    // ---- Display groups ----
    void displayGroupList(int, const std::string&) override {}
    void displayGroupUpdated(int, const std::string&) override {}

    // ---- FA ----
    void receiveFA(faDataType, const std::string&) override {}
    void verifyMessageAPI(const std::string&) override {}
    void verifyCompleted(bool, const std::string&) override {}
    void verifyAndAuthMessageAPI(const std::string&, const std::string&) override {}
    void verifyAndAuthCompleted(bool, const std::string&) override {}
    void replaceFAEnd(int, const std::string&) override {}

    // ---- Historical data (bars & schedule) ----
    void historicalData(TickerId, const Bar&) override {}
    void historicalDataEnd(int, const std::string&, const std::string&) override {}
    void historicalDataUpdate(TickerId, const Bar&) override {}
    void headTimestamp(int, const std::string&) override {}
    void historicalSchedule(int, const std::string&, const std::string&, const std::string&, const std::vector<HistoricalSession>&) override {}

    // ---- Histogram / market rule ----
    void histogramData(int, const HistogramDataVector&) override {}
    void marketRule(int, const std::vector<PriceIncrement>&) override {}

    // ---- Routing ----
    void rerouteMktDataReq(int, int, const std::string&) override {}
    void rerouteMktDepthReq(int, int, const std::string&) override {}

    // ---- Smart components ----
    void smartComponents(int, const SmartComponentsMap&) override {}

    // ---- PnL ----
    void pnl(int, double, double, double) override {}
    void pnlSingle(int, Decimal, double, double, double, double) override {}

    // ---- Historical ticks ----
    void historicalTicks(int, const std::vector<HistoricalTick>&, bool) override {}
    void historicalTicksBidAsk(int, const std::vector<HistoricalTickBidAsk>&, bool) override {}
    void historicalTicksLast(int, const std::vector<HistoricalTickLast>&, bool) override {}

    // ---- Tick-by-tick ----
    void tickByTickAllLast(int, int, time_t, double, Decimal, const TickAttribLast&, const std::string&, const std::string&) override {}
    void tickByTickBidAsk(int, time_t, double, double, Decimal, Decimal, const TickAttribBidAsk&) override {}
    void tickByTickMidPoint(int, time_t, double) override {}

    // ---- IBKR ATS, CME tagging, etc. (covered via existing callbacks) ----

    // ---- User info / metadata / WSH ----
    void userInfo(int, const std::string&) override {}
    void wshMetaData(int, const std::string&) override {}
    void wshEventData(int, const std::string&) override {}

    // ---- Commission & fees ----
    void commissionAndFeesReport(const CommissionAndFeesReport&) override {}

    // ---- Market depth exchanges already above ----

    // ---- ProtoBuf variants (Linux/macOS builds) ----
#if !defined(USE_WIN_DLL)
    void execDetailsProtoBuf(const protobuf::ExecutionDetails&) override {}
    void execDetailsEndProtoBuf(const protobuf::ExecutionDetailsEnd&) override {}
    void orderStatusProtoBuf(const protobuf::OrderStatus&) override {}
    void openOrderProtoBuf(const protobuf::OpenOrder&) override {}
    void openOrdersEndProtoBuf(const protobuf::OpenOrdersEnd&) override {}
    void errorProtoBuf(const protobuf::ErrorMessage&) override {}
#endif
};

