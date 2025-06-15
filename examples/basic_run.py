from market_sim.simulator import MarketSim


sim = MarketSim('AAPL', data= None, expiries=[30], strike_range=[100, 105, 110])

while sim.has_next():
    market = sim.next()
    print(market.underlying)