import json
import time
import os
import boto3
from decimal import Decimal

dynamodb = boto3.resource("dynamodb")
TABLE_NAME = os.environ.get("TABLE_NAME", "BMESensorReadings")
table = dynamodb.Table(TABLE_NAME)

def lambda_handler(event, context):
  print("Incoming event:", json.dumps(event))

  # Case 1: API Gateway HTTP API (JSON is inside event["body"])
  if "body" in event:
    try:
      payload = json.loads(event["body"])
    except json.JSONDecodeError:
      return {
        "statusCode": 400,
        "body": json.dumps({"error": "Invalid JSON inside body"})
      }
  else:
    # Case 2: Direct JSON (Postman, Test button)
    payload = event

  temperature_c = payload.get("temperature_c")
  temperature_f = payload.get("temperature_f")
  device_id = payload.get("device_id", "unknown-device")

  if temperature_c is None or temperature_f is None:
    return {
      "statusCode": 400,
      "body": json.dumps({"error": "Missing temperature values"})
    }

  timestamp = int(time.time())

  try:
    table.put_item(
      Item={
        "deviceId": device_id,
        "timestamp": Decimal(str(timestamp)),
        "temperature_c": Decimal(str(temperature_c)),
        "temperature_f": Decimal(str(temperature_f))
      }
    )

    return {
      "statusCode": 200,
      "body": json.dumps({"message": "Stored"})
    }

  except Exception as e:
    print("DynamoDB error:", str(e))
    return {
      "statusCode": 500,
      "body": json.dumps({"error": "DynamoDB write failed"})
    }
