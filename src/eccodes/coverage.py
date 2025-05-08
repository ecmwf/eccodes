#!/usr/bin/env python3

import argparse
import requests
import json
import os

def print_coverage_branch(headers, endpoint):
    ep = endpoint.format(f"?branch={args.branch}")
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print("Error: Unable to fetch data from Codecov API")
        return

    if response.json()['count'] == 0:
        print(f"No coverage data found for the specified branch: {args.branch}")
        return

    coverage = response.json()['results'][0]['totals']['coverage']
    print(coverage)


def print_coverage_files(headers, endpoint):
    ep = endpoint.format(f"?branch={args.branch}")
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print("Error: Unable to fetch data from Codecov API")
        return

    if response.json()['count'] == 0:
        print(f"No coverage data found for the specified branch: {args.branch}")
        return

    commit_sha = response.json()['results'][0]['commitid']

    ep = endpoint.format(commit_sha)
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print("Error: Unable to fetch data from Codecov API")
        return
    content = json.loads(response.content)

    for file in content['report']['files']:
        print(file['totals']['coverage'], file['name'])


def parse_args():
    parser = argparse.ArgumentParser(description='''Get coverage data from ecCodes.\n
    Get CODECOV_TOKEN from environment variable or pass it as an argument. To get the Codecov token: First, log in to Codecov. Click on your avatar in the top right corner and click on Settings. Under the Access tab on the left navigation menu, click Generate Token.
    '''
    )
    parser.add_argument("branch", nargs="?", type=str, help="Branch to get coverage data from", default="develop")
    parser.add_argument("--token", type=str, help="Codecov token for authentication. Alternatively, set the CODECOV_TOKEN environment variable.", default=os.environ.get("CODECOV_TOKEN"),)
    parser.add_argument("--total", action="store_true", help="Get total coverage data")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    headers = {
        'Authorization': 'bearer {}'.format(args.token),
    }
    endpoint = "https://codecov.io/api/v2/github/ecmwf/repos/eccodes/commits/{}"

    if args.total:
        print_coverage_branch(headers, endpoint)
    else:
        print_coverage_files(headers, endpoint)



