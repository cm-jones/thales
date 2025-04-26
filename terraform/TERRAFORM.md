# AWS Deployment with Terraform

This directory contains Terraform configurations for deploying Thales on AWS.

## Prerequisites

1. AWS CLI installed and configured with appropriate credentials
2. Terraform installed (version >= 1.2.0)
3. Docker and Docker Compose installed
4. An S3 bucket for Terraform state (will be created in the setup step)
5. For GitHub Actions deployment:
   - AWS IAM Role with appropriate permissions
   - GitHub repository secrets configured (see below)

## GitHub Actions Deployment

This project includes automated deployment via GitHub Actions. The workflow will:
- Run on pushes to main and pull requests
- Perform Terraform format checks
- Plan and apply infrastructure changes
- Deploy the application to EC2

### Required GitHub Secrets

Set up the following secrets in your GitHub repository:

1. `AWS_ROLE_ARN` - ARN of the AWS IAM role for GitHub Actions
   ```
   arn:aws:iam::<account-id>:role/GitHubActionsRole
   ```

2. `DB_PASSWORD` - Secure password for the RDS database
   ```
   your-secure-database-password
   ```

3. `SSH_PRIVATE_KEY` - SSH private key for EC2 access
   ```
   -----BEGIN RSA PRIVATE KEY-----
   Your EC2 SSH private key content
   -----END RSA PRIVATE KEY-----
   ```

### IAM Role Setup

Create an AWS IAM role with the following trust relationship:

```json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Principal": {
                "Federated": "arn:aws:iam::<account-id>:oidc-provider/token.actions.githubusercontent.com"
            },
            "Action": "sts:AssumeRoleWithWebIdentity",
            "Condition": {
                "StringLike": {
                    "token.actions.githubusercontent.com:sub": "repo:<org>/<repo>:*"
                }
            }
        }
    ]
}
```

Required IAM permissions:
- AmazonEC2FullAccess
- AmazonRDSFullAccess
- AmazonVPCFullAccess
- IAMFullAccess (or more restricted custom policy)
- S3 access for Terraform state
- DynamoDB access for state locking

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
