#!/bin/bash
set -e

cd "$(git rev-parse --show-toplevel)"
cd docker && docker-compose down

echo "Thales trading containers have been stopped."
