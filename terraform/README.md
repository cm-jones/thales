# AWS Deployment with Terraform

This directory contains Terraform configurations for deploying the Thales trading bot on AWS.

## Prerequisites

1. AWS CLI installed and configured with appropriate credentials
2. Terraform installed (version >= 1.2.0)
3. Docker and Docker Compose installed
4. An S3 bucket for Terraform state (will be created in the setup step)

## Initial Setup

Before running the Terraform configurations, you need to set up the S3 backend:

```bash
# Create S3 bucket for Terraform state
aws s3 mb s3://thales-terraform-state --region us-east-2

# Create DynamoDB table for state locking
aws dynamodb create-table \
  --table-name thales-terraform-locks \
  --attribute-definitions AttributeName=LockID,AttributeType=S \
  --key-schema AttributeName=LockID,KeyType=HASH \
  --billing-mode PAY_PER_REQUEST \
  --region us-east-2
```

## Configuration

1. Create a `terraform.tfvars` file with your desired values (a template is provided)
2. Set the database password securely:
   ```bash
   export TF_VAR_db_password="your-secure-password"
   ```

## Deployment Steps

1. Initialize Terraform:
   ```bash
   terraform init
   ```

2. Review the deployment plan:
   ```bash
   terraform plan
   ```

3. Apply the configuration:
   ```bash
   terraform apply
   ```

4. After successful deployment, Terraform will output:
   - EC2 instance public IP
   - RDS endpoint
   - Grafana URL
   - Prometheus URL
   - SSH connection command

## Resource Management

The deployment creates:
- VPC with public and private subnets
- EC2 t2.micro instance (free tier eligible)
- RDS db.t3.micro instance (free tier eligible)
- Security groups for EC2 and RDS
- Docker containers for:
  - Trading bot application
  - PostgreSQL database
  - Prometheus monitoring
  - Grafana dashboards
  - Node Exporter
  - PostgreSQL Exporter

## Monitoring

Access the monitoring interfaces at:
- Grafana: http://<ec2-ip>:3000 (default credentials: admin/admin)
- Prometheus: http://<ec2-ip>:9090

## Clean Up

To destroy all resources:
```bash
terraform destroy
```

Note: This will delete all resources including the database. Make sure to backup any important data before destroying the infrastructure.

## Security Notes

1. Change the default Grafana password immediately after first login
2. Use a strong database password
3. Consider restricting the security group ingress rules to your IP
4. Enable AWS CloudWatch for additional monitoring if needed
5. Regularly update the AMI and container images for security patches
