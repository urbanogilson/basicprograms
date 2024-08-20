import fakeApi from './api.js';

class CurrencyConverter {
    constructor() {
        this.baseUrl = 'github';
        this.currencies = {};
        this.alerts = [];
        this.baseCurrency = '';
        this.init();
    }

    async init() {
        // const response = await fetch(`${this.baseUrl}${base}`);
        // const data = await response.json();
        const data = await fakeApi.getCurrencyRates();
        this.baseCurrency = data.base;
        this.currencies = data.rates;
        this.updateTargetCurrency();
        this.populateCurrencySelectors();
        this.setupEventListeners();
        this.displayCurrencies();
        this.updateMarketStatus();
        this.statusWatcher = setInterval(() => this.updateMarketStatus(), 60000); // Every 1 minute
        this.rateWatcher = setInterval(() => this.checkCurrencyAlerts(), 60000); // Every 1 minute
    }

    updateTargetCurrency() {
        const label = document.getElementById('target-currency-label');
        label.textContent = `Target Currency in ${this.baseCurrency}`;
    }

    populateCurrencySelectors() {
        const fromCurrency = document.getElementById('from-currency');
        const toCurrency = document.getElementById('to-currency');
        const alertFromCurrency = document.getElementById('alert-from-currency');
        fromCurrency.innerHTML = '';
        toCurrency.innerHTML = '';
        alertFromCurrency.innerHTML = '';
        for (const currency in this.currencies) {
            const option1 = document.createElement('option');
            const option2 = document.createElement('option');
            const alertOption = document.createElement('option');
            option1.value = currency;
            option1.textContent = currency;
            option2.value = currency;
            option2.textContent = currency;
            alertOption.value = currency;
            alertOption.textContent = currency;
            fromCurrency.appendChild(option1);
            toCurrency.appendChild(option2);
            alertFromCurrency.appendChild(alertOption);
        }
    }

    setupEventListeners() {
        document.getElementById('convert-btn').addEventListener('click', () => this.convertCurrency());
        document.getElementById('add-rate-btn').addEventListener('click', () => this.addCurrencyRate());
        document.getElementById('filter-input').addEventListener('input', () => this.filterRates());
        document.getElementById('set-alert-btn').addEventListener('click', () => this.setCurrencyAlert());
    }

    convertCurrency() {
        const amount = document.getElementById('amount').value;
        const fromCurrency = document.getElementById('from-currency').value;
        const toCurrency = document.getElementById('to-currency').value;
        const rate = this.currencies[toCurrency] / this.currencies[fromCurrency];
        const convertedAmount = amount * rate;
        document.getElementById('conversion-result').textContent = `${amount} ${fromCurrency} = ${convertedAmount.toFixed(2)} ${toCurrency}`;
    }

    addCurrencyRate() {
        const targetCurrency = document.getElementById('target-currency').value.toUpperCase();
        const rate = parseFloat(document.getElementById('rate').value);

        if (targetCurrency && !isNaN(rate)) {
            this.currencies[targetCurrency] = rate;
            this.displayCurrencies();
            this.populateCurrencySelectors();
        }
    }

    displayCurrencies(filter = '') {
        const currencyList = document.getElementById('currency-list');
        currencyList.innerHTML = '';
        for (const currencyPair in this.currencies) {
            if (currencyPair.toLowerCase().includes(filter.toLowerCase()) || this.currencies[currencyPair].toString().includes(filter)) {
                const li = document.createElement('li');
                li.textContent = `${currencyPair}: ${this.currencies[currencyPair].toFixed(2)}`;
                currencyList.appendChild(li);
            }
        }
    }

    filterRates() {
        const filter = document.getElementById('filter-input').value;
        this.displayCurrencies(filter);
    }

    updateMarketStatus() {
        const marketStatus = document.getElementById('market-status');
        const hours = new Date().getHours();

        const marketOpen = 9 //9 AM
        const marketClose = 17; // 5 PM

        if (hours >= marketOpen && hours < marketClose) {
            marketStatus.textContent = 'Market is Open';
            marketStatus.style.color = 'green';
        } else {
            marketStatus.textContent = 'Market is Closed';
            marketStatus.style.color = 'red';
        }
    }

    setCurrencyAlert() {
        const fromCurrency = document.getElementById('alert-from-currency').value;
        const targetRate = parseFloat(document.getElementById('alert-rate').value);

        if (fromCurrency && !isNaN(targetRate)) {
            this.alerts.push({ fromCurrency, targetRate });
            document.getElementById('alert-status').textContent = `Alert set for ${fromCurrency} reaching ${targetRate}`;
        }
    }

    async checkCurrencyAlerts() {
        const data = await fakeApi.getCurrencyRates();
        const rates = data.rates;

        this.alerts.forEach((alert, index, object) => {
            const { fromCurrency, targetRate } = alert;
            if (rates[fromCurrency] && rates[fromCurrency] >= targetRate) {
                this.showAlert(`Alert: ${fromCurrency} has reached ${targetRate} ${this.baseCurrency}!`);
                object.splice(index, 1);
            }
        });
    }

    showAlert(message) {
        const alertBanner = document.getElementById('alert-banner');
        alertBanner.textContent = message;
        alertBanner.style.display = 'block';
        setTimeout(() => alertBanner.style.display = 'none', 10000); // Hide after 10 seconds
    }
}

document.addEventListener('DOMContentLoaded', () => {
    new CurrencyConverter();
});
