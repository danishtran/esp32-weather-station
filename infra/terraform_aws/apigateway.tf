# HTTP API
resource "aws_apigatewayv2_api" "sensor_api" {
  name          = var.api_name
  protocol_type = "HTTP"
}

# Lambda integration
resource "aws_apigatewayv2_integration" "sensor_integration" {
  api_id                 = aws_apigatewayv2_api.sensor_api.id
  integration_type       = "AWS_PROXY"
  integration_uri        = aws_lambda_function.store_bme_sensor_reading.invoke_arn
  integration_method     = "POST"
  payload_format_version = "2.0"
}

# Route: POST /sensor-data
resource "aws_apigatewayv2_route" "sensor_route" {
  api_id    = aws_apigatewayv2_api.sensor_api.id
  route_key = "POST ${var.api_route_path}"
  target    = "integrations/${aws_apigatewayv2_integration.sensor_integration.id}"
}

# Default stage
resource "aws_apigatewayv2_stage" "default" {
  api_id      = aws_apigatewayv2_api.sensor_api.id
  name        = "$default"
  auto_deploy = true
}

# Allow API Gateway to invoke your Lambda
resource "aws_lambda_permission" "apigw_invoke" {
  statement_id  = "AllowAPIGatewayInvoke"
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.store_bme_sensor_reading.function_name
  principal     = "apigateway.amazonaws.com"

  # Allow any route / method from this API to call the Lambda
  source_arn = "${aws_apigatewayv2_api.sensor_api.execution_arn}/*/*"
}
