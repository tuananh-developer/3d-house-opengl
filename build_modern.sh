#!/usr/bin/env bash
set -e

cd "$(dirname "$0")/modern_landscape"
make
cp modern_landscape /tmp/modern_app && mv -f /tmp/modern_app ../modern_app
chmod +x ../modern_app
echo "===> Hoàn tất! Bạn có thể chạy:"
echo "     ./modern_app"
echo "  hoặc:"
echo "     ./modern_landscape/modern_landscape"
