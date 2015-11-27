#!/usr/bin/env python3

import sys
import csv
from urllib.request import urlopen, quote

from bs4 import BeautifulSoup


def download_price(product_id):
    url = '{0}{1}'.format(search_url, quote(product_id))
    print('downloading {}'.format(url))
    html = urlopen(url)
    soup = BeautifulSoup(html)
    pricing = soup.find('td', {'class': 'catalog-pricing'})
    if pricing is None:
        print('Unable to extract pricing info, skipping...')
        return None

    price_table = pricing.find('table', {'id': 'pricing'})

    is_next_one = False
    for tr in price_table.find_all('tr'):
        for td in tr.find_all('td'):
            if is_next_one:
                return td.string
            if td.string == '1':  # quantity is 1
                is_next_one = True

    return None


def import_bom(file_name):
    """
    Returns a list of lists containing strings for each cell
    """
    with open(sys.argv[1], 'r') as in_file:
        reader = csv.reader(in_file, delimiter=',', quotechar='"')
        return [row for row in reader]

def export_bom(file_name, bom):
    with open(file_name, 'w') as out_file:
        writer = csv.writer(out_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        for row in bom:
            writer.writerow(row)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: {} <BOM.csv>'.format(sys.argv[0]))
        sys.exit(0)

    bom = import_bom(sys.argv[1])
    search_url = 'http://www.digikey.com/product-search/en?keywords='
    bom[0].append('Single Price')
    product_id_index = bom[0].index('Supplier - Digikey')
    for row in bom[2:]:
        product_id = row[product_id_index]
        libref = row[bom[0].index('LibRef')]
        if not product_id:
            print('No product ID for {}'.format(libref))
            continue
        price = download_price(product_id)
        
        if price is None:
            print('Failed to download price for {}'.format(libref))
        else:
           row.append(price)

    export_bom(sys.argv[1].replace('.csv', '-prices.csv'), bom)
