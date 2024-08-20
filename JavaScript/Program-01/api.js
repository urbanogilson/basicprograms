const fakeApi = {
    getCurrencyRates: async () => {
        await new Promise(resolve => setTimeout(resolve, 100));

        return {
            "timestamp": Date.now(),
            "base": "EUR",
            "date": new Date().toISOString().split('T')[0],
            "rates": {
                "EUR": 1,
                "USD": 1.23396,
                "GBP": 0.87956,
                "DKK": 7.4623,
                "CAD": 1.56003,
                "AUD": 1.57391,
                "JPY": 131.623,
                "INR": 80.8334,
                "CNY": 7.8190,
                "BTC": 0.00002345,
            }
        };
    }
};


export default fakeApi;