#!/usr/bin/env python3

# This script fetches coverage data from Codecov for a specified branch of the ecCodes repository.
# It can be used to get total coverage data or detailed coverage data for individual files.
# It requires a Codecov token for authentication, which can be passed as an argument or set as an environment variable.

# Endpoint reference: https://docs.codecov.com/reference/repos_commits_list

import argparse
import requests
import json
import os

def print_error(endpoint, response, message):
    print(f"Endpoint: {endpoint}")
    print(f"Response status code: {response.status_code}")
    print(f"Response content: {response.content}")
    print(f"Error: {message}")
    return


def print_coverage_branch(headers, endpoint):
    ep = endpoint.format(f"?branch={args.branch}")
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print_error(ep, response, "Unable to fetch data from Codecov API")
        return

    if response.json()['count'] == 0:
        print_error(ep, response, f"No coverage data found for the specified branch: {args.branch}")
        return

    coverage = response.json()['results'][0]['totals']['coverage']
    print(f"Coverage for branch {args.branch}: {coverage:.2f}%")


def print_coverage_files(headers, endpoint, add_link=True):
    ep = endpoint.format(f"?branch={args.branch}")
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print_error(ep, response, "Unable to fetch data from Codecov API")
        return

    if response.json()['count'] == 0:
        print_error(ep, response, f"No coverage data found for the specified branch: {args.branch}")
        return

    commit_sha = response.json()['results'][0]['commitid']

    ep = endpoint.format(commit_sha)
    response = requests.get(ep, headers=headers)
    if response.status_code != 200:
        print_error(ep, response, "Unable to fetch data from Codecov API")
        return
    content = json.loads(response.content)

    link_to_codecov = lambda filename, branch: f"https://codecov.io/gh/ecmwf/eccodes/blob/{branch}/{filename}"

    for file in content['report']['files']:
        output = "{:6.2f} {}".format(file['totals']['coverage'], file['name'])
        if add_link:
            output += " - " + link_to_codecov(file['name'], args.branch)
        print(output)


def parse_args():
    parser = argparse.ArgumentParser(description='''Get coverage data from ecCodes.\n
    Get CODECOV_TOKEN from environment variable or pass it as an argument. To get the Codecov token: First, log in to Codecov. Click on your avatar in the top right corner and click on Settings. Under the Access tab on the left navigation menu, click Generate Token.
    '''
    )
    parser.add_argument("branch", nargs="?", type=str, help="Branch to get coverage data from", default="develop")
    parser.add_argument("--token", type=str, help="Codecov token for authentication. Alternatively, set the CODECOV_TOKEN environment variable.", default=os.environ.get("CODECOV_TOKEN"),)
    parser.add_argument("--total", action="store_true", help="Get total coverage data")
    parser.add_argument("--link", action="store_true", help="Add link to Codecov in the output")
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
        print_coverage_files(headers, endpoint, args.link)



