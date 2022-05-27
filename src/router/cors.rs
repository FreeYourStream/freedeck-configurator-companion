pub fn corsify(request: &rouille::Request, response: rouille::Response) -> rouille::Response {
    return response
        .with_additional_header(
            "Access-Control-Allow-Origin",
            request.header("Origin").unwrap_or("*").to_string(),
        )
        .with_additional_header("Allow", "GET, POST, HEAD, OPTIONS")
        .with_additional_header(
            "Access-Control-Allow-Headers",
            "X-Requested-With, Content-Type, Accept, Origin, Authorization",
        )
        .with_additional_header("Access-Control-Allow-Methods", "OPTIONS, GET, POST, HEAD");
}
