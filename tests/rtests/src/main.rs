use goose::{config::GooseConfiguration, prelude::*};
use std::time::Duration;

#[tokio::main]
async fn main() -> Result<(), GooseError> {
    let mut config = GooseConfiguration::default();
    config.host = "http://localhost:8080".to_string();
    config.run_time = "1m".to_string();
    let attack = GooseAttack::initialize_with_config(config)?;

    attack
        .register_scenario(
            scenario!("TurboAttack")
                .set_wait_time(Duration::from_millis(10), Duration::from_millis(100))?
                .register_transaction(transaction!(website_login).set_on_start())
                .register_transaction(transaction!(website_index))
                .register_transaction(transaction!(long_url))
                .register_transaction(transaction!(super_long_url))
                .register_transaction(transaction!(website_about)),
        )
        .execute()
        .await?;
    Ok(())
}

async fn website_login(user: &mut GooseUser) -> TransactionResult {
    let params = [("username", "test_user"), ("password", "")];
    let _goose = user.post_form("/login", &params).await?;
    Ok(())
}

async fn website_index(user: &mut GooseUser) -> TransactionResult {
    let _goose = user.get("/").await?;
    Ok(())
}

async fn website_about(user: &mut GooseUser) -> TransactionResult {
    let _goose = user.get("/about/").await?;
    Ok(())
}

async fn long_url(user: &mut GooseUser) -> TransactionResult {
    let _goose = user
        .get("/about/this/is/a/very/long/url/that/should/be/shortened")
        .await?;
    Ok(())
}

async fn super_long_url(user: &mut GooseUser) -> TransactionResult {
    let _goose = user
        .get("/about/this/is/a/very/long/url/that/should/be/shortened/and/this/should/be/shortened/too/and/t/an")
        .await?;
    Ok(())
}
