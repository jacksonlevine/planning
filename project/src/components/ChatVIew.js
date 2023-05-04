import { generateUUID } from "three/src/math/MathUtils.js";

export default function ChatView(props) {
    return (
        <div style={{
            zIndex: "10",
            color: "white",
            position: "absolute",
            left: "0",
            top: "50%",
            WebkitTextStroke: "0.5px black",
        }}>
            {props.chats.map((chat) =>
                <p key={generateUUID()}>{chat.id}: {chat.message}</p>
            )}
        </div>
    );
}