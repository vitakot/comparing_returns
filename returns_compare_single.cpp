#include <optional>
#include <fstream>
#include <zorro.h>

/// 1 means long only, -1 means short only
int SIDE = 1;
#define REBALANCE

std::ofstream plotFile;

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

    static std::optional<double> initialInvestment;

    if (is(INITRUN)) {
        asset("BTC-PERP");
#ifdef REBALANCE
        plotFile.open("Log\\returns_compare_rebalance.csv");
#else
        plotFile.open("Log\\returns_compare_allin.csv");
#endif
        plotFile << "timestamp,returns" << std::endl;
    }

    if (!is(LOOKBACK)) {

        if (!initialInvestment) {
#ifdef REBALANCE
            /// We must invest more then 1 lot to run the backtest till the end
            initialInvestment = priceClose(0) * 100;
#else
            /// Start with 1 Lot
            initialInvestment = priceClose(0);
#endif
        }

#ifdef REBALANCE
        Lots = *initialInvestment / priceClose(0) / LotAmount;
#else
        /// We remain invested in the amount of 1 lot for the whole time (All-In)
        Lots = 1;
#endif

        if (SIDE == 1) {
            exitLong();
            enterLong();
        } else if (SIDE == -1) {
            exitShort();
            enterShort();
        }

        double returns = Equity / (*initialInvestment);
        plot("Returns", returns, NEW, BLUE);
        plotFile << utm(wdate(0)) << "," << returns << std::endl;
    }
}