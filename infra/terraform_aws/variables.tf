variable "project_name" {
  type        = string
  default     = "esp32-weather-station"
}

variable "aws_region" {
  type    = string
  default = "us-west-1"
}

variable "lambda_function_name" {
  type    = string
  default = "StoreBMESensorReading"
}

variable "dynamodb_table_name" {
  type    = string
  default = "BMESensorReadings"
}

variable "api_name" {
  type    = string
  default = "bme-sensor-http-api"
}

variable "api_route_path" {
  type    = string
  default = "/sensor-data"
}
