// SPDX-License-Identifier: MIT

use chrono::NaiveDate;

#[derive(Debug, PartialEq)]
pub enum OptionType {
    Call,
    Put,
}

#[derive(Debug)]
pub struct Option {
    pub symbol: String,
    pub type: OptionType,
    pub strike: f64,
    pub expiration: NaiveDate,
    pub size: u32,
}

impl Option {
    pub fn new(symbol: String, type: OptionType, strike: f64, expiration: NaiveDate, size: u32) -> Self {
        Option {
            symbol,
            type,
            strike,
            expiration,
            size,
        }
    }

    pub fn description(&self) -> String {
        format!("{} {} ${} {}",
            self.symbol,
            if self.type == OptionType::Call { "Call" } else { "Put" },
            self.strike,
            self.expiration.format("%Y-%m-%d")
        )
    }
}
