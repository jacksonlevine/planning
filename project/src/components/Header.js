export default function Header() {
    let imgStyle = {
        width: "60px",
        height: "auto",

    }
    let headStyle = {
        alignItems: "center",
        display: "flex",
        flexDirection: "row",
        justifyContent: "center",
        margin: "0 0"
    }
    return (
        <h1 style = {headStyle}><img style={imgStyle} src="logo192.png"/>JacksonGame</h1>
    );
}