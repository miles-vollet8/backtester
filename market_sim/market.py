import datetime
class Option:
    def __init__(self, symbol: str, expiry: date, strike: float, type_:str,
                 bid: float, ask: float, iv: float):
        
        pass

class Market:
    def __init__(self, timestamp: datetime, underlying: float, option_chain: list[Option]):
        self.timestamp = timestamp
        self.underlying = underlying
        self.option_chain = option_chain

    def get_option(self, symbol):
        return next(opt for opt in self.option_chain if opt.symbol == symbol)
        