output "dynamodb_table_name" {
  value = aws_dynamodb_table.sensor_readings.name
}

output "lambda_function_arn" {
  value = aws_lambda_function.store_bme_sensor_reading.arn
}

output "http_api_id" {
  value = aws_apigatewayv2_api.sensor_api.id
}

output "http_api_invoke_url" {
  value = aws_apigatewayv2_api.sensor_api.api_endpoint
}
