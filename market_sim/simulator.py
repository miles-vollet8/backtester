

class MarketSim:
    def __init__(self, 
                tickers: list[str],
                start: date, end: date,
                expiries: list[int],
                strike_range: list[float],
                interval: str = 'daily'):
        for ticker in tickers:
            return False


    
    def has_next(self):
        return False

    def next(self):
        raise NotImplementedError('Not built yet')