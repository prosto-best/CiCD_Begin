#!/bin/bash

scp -i /home/gitlab-runner/.ssh/id_rsa src/cat/s21_cat cicd2@10.20.0.2:/home/cicd2/
scp -i /home/gitlab-runner/.ssh/id_rsa src/grep/s21_grep cicd2@10.20.0.2:/home/cicd2/