export default function Header() {
    let imgStyle = {
        width: "75px",
        height: "auto",

    }
    let headStyle = {
        alignItems: "center",
        display: "flex",
        flexDirection: "row",
        
    }
    return (
        <h1 style = {headStyle}><img style={imgStyle} src="logo192.png"/>JacksonGame</h1>
    );
}