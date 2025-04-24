project_name = "thales"
environment = "prod"
aws_region  = "us-east-2"

vpc_cidr           = "10.0.0.0/16"
public_subnet_cidr = "10.0.1.0/24"
private_subnet_cidr = "10.0.2.0/24"

instance_type = "t2.micro"
db_instance_class = "db.t3.micro"

db_name = "thales"
db_username = "thales"
# db_password should be provided via command line or environment variable
# db_password = "your-secure-password"
