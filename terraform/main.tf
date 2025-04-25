terraform {
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }

  required_version = ">= 1.2.0"

  backend "s3" {
    bucket         = "thales-terraform-state"
    key            = "terraform.tfstate"
    region         = "us-east-2"
    encrypt        = true
    dynamodb_table = "thales-terraform-locks"
  }
}

provider "aws" {
  region = var.aws_region
}

# VPC Configuration
resource "aws_vpc" "thales_vpc" {
  cidr_block           = var.vpc_cidr
  enable_dns_hostnames = true
  enable_dns_support   = true

  tags = {
    Name        = "${var.project_name}-vpc"
    Environment = var.environment
  }
}

# Public Subnet
resource "aws_subnet" "thales_public_subnet" {
  vpc_id                  = aws_vpc.thales_vpc.id
  cidr_block              = var.public_subnet_cidr
  availability_zone       = "${var.aws_region}a"
  map_public_ip_on_launch = true

  tags = {
    Name        = "${var.project_name}-public-subnet"
    Environment = var.environment
  }
}

# Internet Gateway
resource "aws_internet_gateway" "thales_igw" {
  vpc_id = aws_vpc.thales_vpc.id

  tags = {
    Name        = "${var.project_name}-igw"
    Environment = var.environment
  }
}

# Route Table
resource "aws_route_table" "thales_public_rt" {
  vpc_id = aws_vpc.thales_vpc.id

  route {
    cidr_block = "0.0.0.0/0"
    gateway_id = aws_internet_gateway.thales_igw.id
  }

  tags = {
    Name        = "${var.project_name}-public-rt"
    Environment = var.environment
  }
}

# Route Table Association
resource "aws_route_table_association" "thales_public_rt_assoc" {
  subnet_id      = aws_subnet.thales_public_subnet.id
  route_table_id = aws_route_table.thales_public_rt.id
}

# Security Group
resource "aws_security_group" "thales_sg" {
  name        = "${var.project_name}-security-group"
  description = "Security group for ${var.project_name} trading bot"
  vpc_id      = aws_vpc.thales_vpc.id

  # SSH access
  ingress {
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }

  # Grafana access
  ingress {
    from_port   = 3000
    to_port     = 3000
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }

  # Prometheus access
  ingress {
    from_port   = 9090
    to_port     = 9090
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }

  # Allow all outbound traffic
  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }

  tags = {
    Name        = "${var.project_name}-sg"
    Environment = var.environment
  }
}

# EC2 Instance
resource "aws_instance" "thales_instance" {
  ami           = "ami-0430580de6244e02e"  # Ubuntu 24.04 LTS in us-east-2
  instance_type = var.instance_type

  subnet_id                   = aws_subnet.thales_public_subnet.id
  vpc_security_group_ids      = [aws_security_group.thales_sg.id]
  associate_public_ip_address = true

  user_data = <<-EOF
              #!/bin/bash
              apt update
              apt install -y docker.io docker-compose
              systemctl start docker
              systemctl enable docker
              usermod -aG docker ubuntu
              EOF

  tags = {
    Name        = "${var.project_name}-trading-bot"
    Environment = var.environment
  }

  root_block_device {
    volume_size = 8  # Free tier eligible
  }
}

# RDS Instance
resource "aws_db_instance" "thales_db" {
  identifier           = "${var.project_name}-db"
  allocated_storage    = 20
  storage_type        = "gp2"
  engine              = "postgres"
  engine_version      = "15"
  instance_class      = var.db_instance_class
  username            = var.db_username
  password            = var.db_password
  skip_final_snapshot = true

  vpc_security_group_ids = [aws_security_group.thales_db_sg.id]
  db_subnet_group_name   = aws_db_subnet_group.thales_db_subnet_group.name

  tags = {
    Name        = "${var.project_name}-db"
    Environment = var.environment
  }
}

# RDS Security Group
resource "aws_security_group" "thales_db_sg" {
  name        = "thales-db-security-group"
  description = "Security group for Thales RDS instance"
  vpc_id      = aws_vpc.thales_vpc.id

  ingress {
    from_port       = 5432
    to_port         = 5432
    protocol        = "tcp"
    security_groups = [aws_security_group.thales_sg.id]
  }

  tags = {
    Name        = "${var.project_name}-db-sg"
    Environment = var.environment
  }
}

# DB Subnet Group
resource "aws_db_subnet_group" "thales_db_subnet_group" {
  name       = "thales-db-subnet-group"
  subnet_ids = [aws_subnet.thales_public_subnet.id, aws_subnet.thales_private_subnet.id]

  tags = {
    Name        = "${var.project_name}-db-subnet-group"
    Environment = var.environment
  }
}

# Private Subnet for RDS
resource "aws_subnet" "thales_private_subnet" {
  vpc_id            = aws_vpc.thales_vpc.id
  cidr_block        = var.private_subnet_cidr
  availability_zone = "${var.aws_region}b"

  tags = {
    Name        = "${var.project_name}-private-subnet"
    Environment = var.environment
  }
}
