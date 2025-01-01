const cheerio = require('cheerio');
const playwright = require('playwright');
const fs = require('fs');
const path = require('path');

const saveDir = 'tempFlightData';
const saveFile = 'flightData.json';


const scoot_flights = async () => {
  const browser = await playwright.chromium.launch({ headless: false });
  const context = await browser.newContext();
  const page = await context.newPage();

  const dep = process.env.DepENV;
  const dest = process.env.DestENV;
  const ld = process.env.LeaveENV;
  const rd = process.env.ReturnENV

  const temp = "https://www.google.com/travel/flights";

  await page.goto(temp);

  const fromInput = await page.$('input[placeholder="Where from?"]');
  await fromInput.fill(dep);
  await page.waitForTimeout(1000);
  await page.keyboard.press('Enter');
  await page.waitForTimeout(1000);

  const toInput = await page.$('input[placeholder="Where to?"]');
  await toInput.fill(dest);
  await page.waitForTimeout(1000);
  await page.keyboard.press('Enter');
  await page.waitForTimeout(1000);

  const ldInput = await page.$('input[placeholder="Departure"]');
  await ldInput.fill(ld);
  await page.waitForTimeout(1000);
  await page.keyboard.press('Enter');
  await page.waitForTimeout(1000);

  const rdInput = await page.$('input[placeholder="Return"]');
  await rdInput.fill(rd);
  await page.waitForTimeout(1000);
  await page.keyboard.press('Enter');
  await page.waitForTimeout(1000);

  await page.click('button:has-text("Search")');

  await page.waitForTimeout(1000);
  await page.waitForSelector('.pIav2d');

  const htmlContent = await page.content();

  await browser.close();

  let prices = [];

  const $ = cheerio.load(htmlContent);

  $('.pIav2d').each((index, element) => {

    const airline = $(element).find('.sSHqwe span').last().text().trim();
    const price = $(element).find('.YMlIz span').last().text().trim();

    prices.push({ airline, price });
  });

  prices = prices.slice(0, prices.length / 2);
  prices.sort((a,b) => a.price - b.price);

  const save = () => {
    try {
      fs.writeFileSync(path.join(saveDir, saveFile), JSON.stringify(prices, null, 1));
    } catch {
      fs.mkdirSync(saveDir);
      fs.writeFileSync(path.join(saveDir, saveFile), JSON.stringify(prices, null, 1));
    }
  };

  save();
  

}

scoot_flights();
