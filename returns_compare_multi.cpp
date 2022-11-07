#include <optional>
#include <string>
#include <map>
#include <zorro.h>

constexpr auto ASSET_LIST = R"(C:\Algo\Zorro\Lists\AssetsFTXPerpsBTCXRP.csv)";
const double investmentPerAsset = 100;
std::map<std::string, int> lotsPerAsset;
std::map<std::string, double> weights;
double initialInvestedSum = 0;

/// 1 means long only, -1 means short only
int SIDE = 1;
//#define REBALANCE

DLLFUNC void run() {
    set(PLOTNOW, LOGFILE, PRELOAD, NFA);
    setf(PlotMode, PL_ALL + PL_FINE);
    setf(TradeMode, TR_GTC);
    SaveMode = SV_SLIDERS + SV_ALGOVARS + SV_TRADES + SV_BACKUP;
    BarPeriod = 1440;
    BarMode = BR_WEEKEND + BR_MARKET + BR_SLEEP + BR_LOGOFF;
    MonteCarlo = 0;
    StopFactor = 0;
    StartDate = 20200719;
    Hedge = 0;
    BarZone = UTC;
    LookBack = 0;
    BarOffset = 0;
    StartMarket = EndMarket = 0;
    resf(BarMode, BR_WEEKEND);
    Fill = 0;
    Spread = Commission = RollLong = RollShort = Slippage = 0;

    if (is(INITRUN)) {
        assetList(ASSET_LIST);
        weights.insert_or_assign("BTC-PERP", 1);
        weights.insert_or_assign("XRP-PERP", 1);
    }

    while (asset(loop(Assets))) {
        if (!is(LOOKBACK)) {

            auto it = lotsPerAsset.find(Asset);

            if (it != lotsPerAsset.end()) {
                Lots = it->second;
            } else {
                double invested = investmentPerAsset * weights.find(Asset)->second;
                initialInvestedSum += invested;
                int lotsForAsset = invested / priceClose(0) / LotAmount;
                lotsPerAsset.insert_or_assign(Asset, lotsForAsset);
                Lots = lotsForAsset;
            }

            if (SIDE == 1) {
                exitLong();
                enterLong();
            } else if (SIDE == -1) {
                exitShort();
                enterShort();
            }
        }
    }

    double profit = Equity / initialInvestedSum;
    plot("Profit", profit, NEW, BLUE);
}