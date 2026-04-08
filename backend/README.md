# Backend (Spring Boot) Scaffold

This directory will contain the Spring Boot backend for Electric-Nose.

## Structure (planned)

```
backend/
  src/main/java/com/electricnose/
    api/
    config/
    domain/
    dto/
    repository/
    service/
    integration/
  src/main/resources/
    application.yml
    db/migration/
  src/test/java/
  pom.xml
```

## Environment variables

Copy `.env.example` to `.env` and fill values. Use a secrets manager for production.

## Run (after project generation)

```
mvn spring-boot:run
```


