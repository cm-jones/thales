output "ec2_instance_public_ip" {
  description = "Public IP address of the EC2 instance"
  value       = aws_instance.thales_instance.public_ip
}

output "rds_endpoint" {
  description = "RDS instance endpoint"
  value       = aws_db_instance.thales_db.endpoint
}

output "grafana_url" {
  description = "URL to access Grafana dashboard"
  value       = "http://${aws_instance.thales_instance.public_ip}:3000"
}

output "prometheus_url" {
  description = "URL to access Prometheus"
  value       = "http://${aws_instance.thales_instance.public_ip}:9090"
}

output "ssh_connection" {
  description = "SSH command to connect to the instance"
  value       = "ssh ubuntu@${aws_instance.thales_instance.public_ip}"
}
